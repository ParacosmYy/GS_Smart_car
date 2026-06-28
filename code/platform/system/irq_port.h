#ifndef IRQ_PORT_H_
#define IRQ_PORT_H_

/**
 * @file irq_port.h
 * @brief 中断端口契约（Platform contract）。
 */

#include <stdint.h>
#include "irq_fact.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 中断源编号（由 Impl 端口定义） */
typedef uint16_t irq_source_t;

/** @brief 中断 handler 签名 */
typedef irq_fact_t (*irq_handler_t)(void);

/** @brief 端口分发回调类型 */
typedef void (*irq_port_dispatch_fn_t)(irq_source_t source);

/** @brief 端口路由表项 */
typedef struct
{
    irq_source_t  source;
    irq_handler_t handler;
    irq_fact_t    fact_mask;
} irq_port_route_t;

/** @brief 注册中断分发回调 */
void irq_port_register_dispatch(irq_port_dispatch_fn_t fn);

/** @brief 分发中断源 */
void irq_port_dispatch(irq_source_t source);

/** @brief 获取端口路由表 */
const irq_port_route_t *irq_port_get_routes(uint16_t *p_count);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_PORT_H_ */
