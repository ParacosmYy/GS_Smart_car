/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file irq_port_adapter.c
 *
 * @par dependencies
 * - irq_port_adapter.h
 * - smartcar_irq_router.h
 * - irq_port.h
 *
 * @author GS_Mark
 *
 * @brief IRQ port adapter —— 桥接 Platform Port 契约 与 System IRQ Router。
 *
 * Processing flow:
 * The adapter reads the irq_port_route_t table from the Impl layer (e.g.
 * tc264_irq_port.c), combines it with system configuration (scheduler events
 * and tick_ms mappings), constructs a full smartcar_irq_route_t table, and
 * registers it with SmartcarIrqRouter. Finally it sets the global dispatch
 * callback so Impl ISR entry points can route interrupts without depending
 * on System headers at compile time.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "irq_port_adapter.h"

#include "platform/system/irq_port.h"
#include "smartcar_irq_router.h"
#include "event.h"
#include "config.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define IRQ_PORT_ADAPTER_MAX_ROUTES    (32U)
//******************************** Defines **********************************//

//******************************** Variables ********************************//

/**
 * @brief 完整系统路由表缓冲区（port route 扩展后的结果）。
 *
 * 在 IrqPortAdapter_Init() 中填充，生命周期为整个系统运行期间。
 */
static smartcar_irq_route_t s_sys_routes[IRQ_PORT_ADAPTER_MAX_ROUTES];
//******************************** Variables ********************************//

//******************************** Implement ********************************//

/**
 * @brief 初始化 IRQ port adapter。
 *
 * 处理步骤：
 *  1. 通过 irq_port_get_routes() 获取 Impl 层端口路由表。
 *  2. 对每条 port route，结合内置 system 配置（event/tick 映射表）
 *     构造完整的 smartcar_irq_route_t。
 *  3. 调用 SmartcarIrqRouter_Init() 注册路由。
 *  4. 通过 irq_port_register_dispatch() 注入分发回调。
 *
 * @return void : 无返回值。
 *
 * */
void IrqPortAdapter_Init(void)
{
    uint16_t port_count = 0U;
    uint16_t i = 0U;
    const irq_port_route_t *p_port_routes = irq_port_get_routes(&port_count);
    uint16_t sys_count = 0U;

    if ((p_port_routes == 0) || (port_count == 0U)
        || (port_count > IRQ_PORT_ADAPTER_MAX_ROUTES))
    {
        return;
    }

    /* 转换：port route (source, handler, fact_mask) → sys route (+ events, tick) */
    for (i = 0U; i < port_count; i++)
    {
        s_sys_routes[i].source = p_port_routes[i].source;
        s_sys_routes[i].handler = p_port_routes[i].handler;
        s_sys_routes[i].fact_mask = p_port_routes[i].fact_mask;

        /* 根据 fact_mask 映射调度事件和时间基 */
        switch (p_port_routes[i].fact_mask)
        {
        case IRQ_FACT_ENCODER_WINDOW:
            s_sys_routes[i].scheduler_events = EVT_ENCODER_50MS;
            s_sys_routes[i].tick_ms = 0U;
            break;

        case IRQ_FACT_GYRO_TICK:
            s_sys_routes[i].scheduler_events = EVT_GYRO_10MS;
            s_sys_routes[i].tick_ms = PIT_PERIOD_MS;
            break;

        case IRQ_FACT_CAMERA_FRAME:
            s_sys_routes[i].scheduler_events = EVT_CAM_FRAME;
            s_sys_routes[i].tick_ms = 0U;
            break;

        default: /* IRQ_FACT_NONE 或未识别的事实 */
            s_sys_routes[i].scheduler_events = EVT_NONE;
            s_sys_routes[i].tick_ms = 0U;
            break;
        }

        sys_count++;
    }

    SmartcarIrqRouter_Init(s_sys_routes, sys_count);
    irq_port_register_dispatch(SmartcarIrqRouter_Dispatch);
}
//******************************** Implement ********************************//
