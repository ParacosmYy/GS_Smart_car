#ifndef PAL_IRQ_PORT_H_
#define PAL_IRQ_PORT_H_

/**
 * @file pal_irq_port.h
 * @brief LEGACY 中断端口兼容接口。
 */

#include "irq_port.h"

typedef irq_source_t pal_irq_source_t;
typedef irq_handler_t pal_irq_handler_t;
typedef irq_port_dispatch_fn_t pal_irq_port_dispatch_fn_t;
typedef irq_port_route_t pal_irq_port_route_t;

#define pal_irq_port_register_dispatch irq_port_register_dispatch
#define pal_irq_port_dispatch          irq_port_dispatch
#define pal_irq_port_get_routes        irq_port_get_routes

#endif /* PAL_IRQ_PORT_H_ */
