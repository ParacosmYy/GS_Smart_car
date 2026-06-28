/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file smartcar_irq_router.c
 *
 * @par dependencies
 * - smartcar_irq_router.h
 * - config.h
 * - event.h
 * - impl/tc264/isr_adapter.h
 * - scheduler.h
 *
 * @author GS_Mark
 *
 * @brief Smart car system interrupt router implementation.
 *
 * Processing flow:
 * TC264 ISR entries only identify the interrupt source. This system router
 * dispatches the source through a static route table, calls the target ISR
 * adapter, then publishes scheduler events and time-base ticks.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "smartcar_irq_router.h"

#include "config.h"
#include "event.h"
#include "impl/tc264/isr_adapter.h"
#include "scheduler.h"
//******************************** Includes *********************************//

//******************************** Types ************************************//
typedef isr_adapter_event_t (*smartcar_irq_adapter_fn_t)(void);

typedef struct
{
    smartcar_irq_source_t       source;
    smartcar_irq_adapter_fn_t   handler;
    isr_adapter_event_t         adapter_event_mask;
    event_mask_t                scheduler_events;
    uint32_t                    tick_ms;
} smartcar_irq_route_t;

typedef struct
{
    volatile uint32_t source_count[SMARTCAR_IRQ_SOURCE_MAX];
    volatile uint32_t dispatch_count;
    volatile uint32_t invalid_source_count;
} smartcar_irq_router_context_t;
//******************************** Types ************************************//

//******************************** Declaring ********************************//
static void SmartcarIrqRouter_PublishRoute(const smartcar_irq_route_t *p_route,
                                           isr_adapter_event_t adapter_events);
//******************************** Declaring ********************************//

//******************************** Variables ********************************//
static smartcar_irq_router_context_t s_irq_router_ctx =
{
    {0U},
    0U,
    0U
};

static const smartcar_irq_route_t s_irq_route_table[SMARTCAR_IRQ_SOURCE_MAX] =
{
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0, ISR_ADAPTER_EVT_ENCODER_WINDOW, EVT_ENCODER_50MS, 0U},
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1, ISR_ADAPTER_EVT_GYRO_TICK, EVT_GYRO_10MS, PIT_PERIOD_MS},
    {SMARTCAR_IRQ_SOURCE_CCU61_PIT_CH0, IsrAdapter_Ccu61PitCh0, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_CCU61_PIT_CH1, IsrAdapter_Ccu61PitCh1, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_EXTI_CH0_CH4, IsrAdapter_ExtiCh0Ch4, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_EXTI_CH1_CH5, IsrAdapter_ExtiCh1Ch5, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_EXTI_CH3_CH7, IsrAdapter_ExtiCh3Ch7, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_DMA_CH5, IsrAdapter_DmaCh5, ISR_ADAPTER_EVT_CAMERA_FRAME, EVT_CAM_FRAME, 0U},
    {SMARTCAR_IRQ_SOURCE_UART0_TX, IsrAdapter_Uart0Tx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART0_RX, IsrAdapter_Uart0Rx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART1_TX, IsrAdapter_Uart1Tx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART1_RX, IsrAdapter_Uart1Rx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART2_TX, IsrAdapter_Uart2Tx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART2_RX, IsrAdapter_Uart2Rx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART3_TX, IsrAdapter_Uart3Tx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART3_RX, IsrAdapter_Uart3Rx, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART0_ERROR, IsrAdapter_Uart0Error, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART1_ERROR, IsrAdapter_Uart1Error, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART2_ERROR, IsrAdapter_Uart2Error, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART3_ERROR, IsrAdapter_Uart3Error, ISR_ADAPTER_EVT_NONE, EVT_NONE, 0U}
};
//******************************** Variables ********************************//

//******************************** Implement ********************************//
/**
 * @brief 发布路由表描述的调度事件和时间基。
 *
 * 处理步骤：
 *  1. 仅在平台事件命中时推进调度器时间基。
 *  2. 仅在平台事件命中时发布对应调度事件。
 *
 * @param[in] p_route        : 中断路由描述。
 * @param[in] adapter_events : 平台适配层返回的事件。
 *
 * @return void : 无返回值。
 *
 * */
static void SmartcarIrqRouter_PublishRoute(const smartcar_irq_route_t *p_route,
                                           isr_adapter_event_t adapter_events)
{
    uint8_t has_adapter_event = 0U;

    if (p_route == 0)
    {
        return;
    }

    if (p_route->adapter_event_mask == ISR_ADAPTER_EVT_NONE)
    {
        has_adapter_event = 1U;
    }
    else if ((adapter_events & p_route->adapter_event_mask) != 0U)
    {
        has_adapter_event = 1U;
    }

    if (has_adapter_event == 0U)
    {
        return;
    }

    if (p_route->tick_ms != 0U)
    {
        Scheduler_AddTickFromIsr(p_route->tick_ms);
    }

    if (p_route->scheduler_events != EVT_NONE)
    {
        event_post_from_isr(p_route->scheduler_events);
    }
}

/**
 * @brief 按中断源分发 ISR 后半段处理。
 *
 * 处理步骤：
 *  1. 校验 source 是否在路由表范围内。
 *  2. 调用对应平台 ISR adapter。
 *  3. 按路由表发布调度事件和系统时间基。
 *
 * @param[in] source : Smart car 中断源编号。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarIrqRouter_Dispatch(smartcar_irq_source_t source)
{
    const smartcar_irq_route_t *p_route = 0;
    isr_adapter_event_t adapter_events = ISR_ADAPTER_EVT_NONE;

    if (source >= SMARTCAR_IRQ_SOURCE_MAX)
    {
        s_irq_router_ctx.invalid_source_count++;
        return;
    }

    p_route = &s_irq_route_table[source];
    if ((p_route->source != source) || (p_route->handler == 0))
    {
        s_irq_router_ctx.invalid_source_count++;
        return;
    }

    s_irq_router_ctx.dispatch_count++;
    s_irq_router_ctx.source_count[source]++;

    adapter_events = p_route->handler();
    SmartcarIrqRouter_PublishRoute(p_route, adapter_events);
}
//******************************** Implement ********************************//
