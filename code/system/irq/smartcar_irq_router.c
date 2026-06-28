/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file smartcar_irq_router.c
 *
 * @par dependencies
 * - smartcar_irq_router.h
 * - event.h
 * - scheduler.h
 *
 * @author GS_Mark
 *
 * @brief Smart car system interrupt router implementation.
 *
 * Processing flow:
 * SDK ISR entries identify a target source id. This router looks up the source
 * in a target-provided static route table, calls the bound handler, then
 * publishes scheduler events and time-base ticks from returned interrupt facts.
 *
 * @version V1.1 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "smartcar_irq_router.h"

#include "event.h"
#include "scheduler.h"
//******************************** Includes *********************************//

//******************************** Types ************************************//
typedef struct
{
    const smartcar_irq_route_t *p_routes;
    uint16_t                    route_count;
    volatile uint32_t           dispatch_count;
    volatile uint32_t           invalid_source_count;
} smartcar_irq_router_context_t;
//******************************** Types ************************************//

//******************************** Declaring ********************************//
static const smartcar_irq_route_t *SmartcarIrqRouter_FindRoute(smartcar_irq_source_t source);
static void SmartcarIrqRouter_PublishRoute(const smartcar_irq_route_t *p_route,
                                           smartcar_irq_fact_t facts);
//******************************** Declaring ********************************//

//******************************** Variables ********************************//
static smartcar_irq_router_context_t s_irq_router_ctx =
{
    0,
    0U,
    0U,
    0U
};
//******************************** Variables ********************************//

//******************************** Implement ********************************//
/**
 * @brief 初始化中断路由表。
 *
 * 处理步骤：
 *  1. 保存目标平台提供的静态路由表地址。
 *  2. 保存路由表项数量，供后续 source 查找使用。
 *
 * @param[in] p_routes    : 目标平台路由表。
 * @param[in] route_count : 路由表项数量。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarIrqRouter_Init(const smartcar_irq_route_t *p_routes, uint16_t route_count)
{
    s_irq_router_ctx.p_routes = p_routes;
    s_irq_router_ctx.route_count = route_count;
    s_irq_router_ctx.dispatch_count = 0U;
    s_irq_router_ctx.invalid_source_count = 0U;
}

/**
 * @brief 查找中断源对应的路由项。
 *
 * 处理步骤：
 *  1. 遍历目标平台注册的路由表。
 *  2. 返回 source 匹配的路由描述。
 *
 * @param[in] source : 目标平台中断源编号。
 *
 * @return const smartcar_irq_route_t* : 命中的路由项，未命中返回空指针。
 *
 * */
static const smartcar_irq_route_t *SmartcarIrqRouter_FindRoute(smartcar_irq_source_t source)
{
    uint16_t i = 0U;

    if (s_irq_router_ctx.p_routes == 0)
    {
        return 0;
    }

    for (i = 0U; i < s_irq_router_ctx.route_count; i++)
    {
        if (s_irq_router_ctx.p_routes[i].source == source)
        {
            return &s_irq_router_ctx.p_routes[i];
        }
    }

    return 0;
}

/**
 * @brief 发布路由表描述的调度事件和时间基。
 *
 * 处理步骤：
 *  1. 仅在中断事实命中时推进调度器时间基。
 *  2. 仅在中断事实命中时发布对应调度事件。
 *
 * @param[in] p_route : 中断路由描述。
 * @param[in] facts   : 目标适配层返回的中断事实。
 *
 * @return void : 无返回值。
 *
 * */
static void SmartcarIrqRouter_PublishRoute(const smartcar_irq_route_t *p_route,
                                           smartcar_irq_fact_t facts)
{
    uint8_t has_fact = 0U;

    if (p_route == 0)
    {
        return;
    }

    if (p_route->fact_mask == SMARTCAR_IRQ_FACT_NONE)
    {
        has_fact = 1U;
    }
    else if ((facts & p_route->fact_mask) != 0U)
    {
        has_fact = 1U;
    }

    if (has_fact == 0U)
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
 *  1. 查找 source 对应的路由项。
 *  2. 调用目标平台 handler，得到中断事实。
 *  3. 按路由表发布调度事件和系统时间基。
 *
 * @param[in] source : 目标平台中断源编号。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarIrqRouter_Dispatch(smartcar_irq_source_t source)
{
    const smartcar_irq_route_t *p_route = 0;
    smartcar_irq_fact_t facts = SMARTCAR_IRQ_FACT_NONE;

    p_route = SmartcarIrqRouter_FindRoute(source);
    if ((p_route == 0) || (p_route->handler == 0))
    {
        s_irq_router_ctx.invalid_source_count++;
        return;
    }

    s_irq_router_ctx.dispatch_count++;
    facts = p_route->handler();
    SmartcarIrqRouter_PublishRoute(p_route, facts);
}
//******************************** Implement ********************************//
