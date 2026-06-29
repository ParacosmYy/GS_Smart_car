#ifndef PLATFORM_TARGET_PLATFORM_H_
#define PLATFORM_TARGET_PLATFORM_H_

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

typedef irq_fact_t (*target_irq_handler_t)(void);

typedef struct
{
    smartcar_irq_source_t source;
    target_irq_handler_t  handler;
    irq_fact_t            fact_mask;
    uint32_t              event;
    uint32_t              tick_ms;
} target_irq_route_t;

const target_irq_route_t *TargetPlatform_GetIrqRoutes(uint16_t *p_count);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_TARGET_PLATFORM_H_ */
