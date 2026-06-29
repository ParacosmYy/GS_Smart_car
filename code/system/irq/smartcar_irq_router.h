#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

#include <stdint.h>
#include "platform/system/irq_fact.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t smartcar_irq_source_t;

typedef enum
{
    SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH0 = 0,
    SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1,
    SMARTCAR_IRQ_SOURCE_EXTI_CH3_CH7,
    SMARTCAR_IRQ_SOURCE_DMA_CH5,
    SMARTCAR_IRQ_SOURCE_UART0_RX,
    SMARTCAR_IRQ_SOURCE_UART1_RX,
    SMARTCAR_IRQ_SOURCE_UART3_RX,
    SMARTCAR_IRQ_SOURCE_UART0_ERROR,
    SMARTCAR_IRQ_SOURCE_UART1_ERROR,
    SMARTCAR_IRQ_SOURCE_UART3_ERROR
} smartcar_irq_source_enum_t;

typedef irq_fact_t (*smartcar_irq_handler_t)(void);

typedef struct
{
    smartcar_irq_source_t   source;
    smartcar_irq_handler_t  handler;
} smartcar_irq_target_route_t;

const smartcar_irq_target_route_t *TargetPlatform_GetIrqRoutes(uint16_t *p_count);
void SmartcarIrq_Dispatch(smartcar_irq_source_t source);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
