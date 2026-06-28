#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

/**
 * @file smartcar_irq_router.h
 * @brief Smart car system interrupt router interface.
 */

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Includes **********************************//
#include <stdint.h>
#include "event.h"
#include "smartcar_irq_fact.h"
//******************************* Includes **********************************//

//******************************* Types *************************************//
typedef uint16_t smartcar_irq_source_t;
typedef smartcar_irq_fact_t (*smartcar_irq_handler_t)(void);

typedef struct
{
    smartcar_irq_source_t  source;
    smartcar_irq_handler_t handler;
    smartcar_irq_fact_t    fact_mask;
    event_mask_t           scheduler_events;
    uint32_t               tick_ms;
} smartcar_irq_route_t;
//******************************* Types *************************************//

//******************************* Declaring *********************************//
/** @brief 初始化中断路由表 */
void SmartcarIrqRouter_Init(const smartcar_irq_route_t *p_routes, uint16_t route_count);

/** @brief 按中断源分发 ISR 后半段处理 */
void SmartcarIrqRouter_Dispatch(smartcar_irq_source_t source);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
