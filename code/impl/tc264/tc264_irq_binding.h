#ifndef TC264_IRQ_BINDING_H_
#define TC264_IRQ_BINDING_H_

/**
 * @file tc264_irq_binding.h
 * @brief TC264 interrupt source binding.
 */

//******************************* Includes **********************************//
#include "system/irq/smartcar_irq_router.h"
//******************************* Includes **********************************//

//******************************* Types *************************************//
typedef enum
{
    TC264_IRQ_SOURCE_CCU60_PIT_CH0 = 0,
    TC264_IRQ_SOURCE_CCU60_PIT_CH1,
    TC264_IRQ_SOURCE_CCU61_PIT_CH0,
    TC264_IRQ_SOURCE_CCU61_PIT_CH1,
    TC264_IRQ_SOURCE_EXTI_CH0_CH4,
    TC264_IRQ_SOURCE_EXTI_CH1_CH5,
    TC264_IRQ_SOURCE_EXTI_CH3_CH7,
    TC264_IRQ_SOURCE_DMA_CH5,
    TC264_IRQ_SOURCE_UART0_TX,
    TC264_IRQ_SOURCE_UART0_RX,
    TC264_IRQ_SOURCE_UART1_TX,
    TC264_IRQ_SOURCE_UART1_RX,
    TC264_IRQ_SOURCE_UART2_TX,
    TC264_IRQ_SOURCE_UART2_RX,
    TC264_IRQ_SOURCE_UART3_TX,
    TC264_IRQ_SOURCE_UART3_RX,
    TC264_IRQ_SOURCE_UART0_ERROR,
    TC264_IRQ_SOURCE_UART1_ERROR,
    TC264_IRQ_SOURCE_UART2_ERROR,
    TC264_IRQ_SOURCE_UART3_ERROR,
    TC264_IRQ_SOURCE_MAX
} tc264_irq_source_t;
//******************************* Types *************************************//

//******************************* Declaring *********************************//
/** @brief 注册 TC264 中断路由表 */
void Tc264IrqBinding_Init(void);

#endif /* TC264_IRQ_BINDING_H_ */
