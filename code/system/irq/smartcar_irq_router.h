#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

/**
 * @file smartcar_irq_router.h
 * @brief Smart car system interrupt router interface (System layer).
 *
 * The router owns the dispatch loop: it receives a source number from
 * the Impl IRQ port, looks up the route, calls the handler, and publishes
 * scheduler events / tick_ms. It depends on Platform types via irq_port.h.
 */

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Includes **********************************//
#include <stdint.h>
#include "event.h"
#include "platform/system/irq_port.h"
//******************************* Includes **********************************//

//******************************* Types *************************************//
/**
 * @brief 系统级中断路由表项（扩展了 Platform port route）。
 *
 * 在 irq_port_route_t 基础上增加了调度事件和时间基配置，
 * 由 IrqPortAdapter 在启动时从 port route + system 配置合并构造。
 */
typedef struct
{
    irq_source_t      source;           /**< 中断源编号 */
    irq_handler_t     handler;          /**< 中断 handler（adapter 入口） */
    irq_fact_t        fact_mask;        /**< 关注的事实掩码 */
    event_mask_t      scheduler_events; /**< 事实命中时发布的调度事件 */
    uint32_t          tick_ms;          /**< 事实命中时推进的时间基 (ms) */
} smartcar_irq_route_t;
//******************************* Types *************************************//

//******************************* Declaring *********************************//
/** @brief 初始化中断路由表 */
void SmartcarIrqRouter_Init(const smartcar_irq_route_t *p_routes, uint16_t route_count);

/** @brief 按中断源分发 ISR 后半段处理 */
void SmartcarIrqRouter_Dispatch(irq_source_t source);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
