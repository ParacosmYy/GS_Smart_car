/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file smartcar_irq_router.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author GS_Mark
 *
 * @brief Smart car interrupt router interface.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Types *************************************//
typedef enum
{
    SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH0 = 0,
    SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1,
    SMARTCAR_IRQ_SOURCE_CCU61_PIT_CH0,
    SMARTCAR_IRQ_SOURCE_CCU61_PIT_CH1,
    SMARTCAR_IRQ_SOURCE_EXTI_CH0_CH4,
    SMARTCAR_IRQ_SOURCE_EXTI_CH1_CH5,
    SMARTCAR_IRQ_SOURCE_EXTI_CH3_CH7,
    SMARTCAR_IRQ_SOURCE_DMA_CH5,
    SMARTCAR_IRQ_SOURCE_UART0_TX,
    SMARTCAR_IRQ_SOURCE_UART0_RX,
    SMARTCAR_IRQ_SOURCE_UART1_TX,
    SMARTCAR_IRQ_SOURCE_UART1_RX,
    SMARTCAR_IRQ_SOURCE_UART2_TX,
    SMARTCAR_IRQ_SOURCE_UART2_RX,
    SMARTCAR_IRQ_SOURCE_UART3_TX,
    SMARTCAR_IRQ_SOURCE_UART3_RX,
    SMARTCAR_IRQ_SOURCE_UART0_ERROR,
    SMARTCAR_IRQ_SOURCE_UART1_ERROR,
    SMARTCAR_IRQ_SOURCE_UART2_ERROR,
    SMARTCAR_IRQ_SOURCE_UART3_ERROR,
    SMARTCAR_IRQ_SOURCE_MAX
} smartcar_irq_source_t;
//******************************* Types *************************************//

//******************************* Declaring *********************************//
/**
 * @brief 按中断源分发 ISR 后半段处理
 */
void SmartcarIrqRouter_Dispatch(smartcar_irq_source_t source);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
