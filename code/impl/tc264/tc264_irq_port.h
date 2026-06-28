#ifndef TC264_IRQ_PORT_H_
#define TC264_IRQ_PORT_H_

/**
 * @file tc264_irq_port.h
 * @brief TC264 中断入口端口。
 *
 * 路由表暴露通过 irq_port_get_routes()（Platform contract），
 * 由 System 层 IrqPortAdapter 在启动时读取。
 */

/** @brief CCU60 PIT CH0 中断入口 */
void Tc264IrqPort_OnCcu60PitCh0(void);

/** @brief CCU60 PIT CH1 中断入口 */
void Tc264IrqPort_OnCcu60PitCh1(void);

/** @brief CCU61 PIT CH0 中断入口 */
void Tc264IrqPort_OnCcu61PitCh0(void);

/** @brief CCU61 PIT CH1 中断入口 */
void Tc264IrqPort_OnCcu61PitCh1(void);

/** @brief ERU CH0/CH4 中断入口 */
void Tc264IrqPort_OnExtiCh0Ch4(void);

/** @brief ERU CH1/CH5 中断入口 */
void Tc264IrqPort_OnExtiCh1Ch5(void);

/** @brief ERU CH3/CH7 中断入口 */
void Tc264IrqPort_OnExtiCh3Ch7(void);

/** @brief DMA CH5 中断入口 */
void Tc264IrqPort_OnDmaCh5(void);

/** @brief UART0 TX 中断入口 */
void Tc264IrqPort_OnUart0Tx(void);

/** @brief UART0 RX 中断入口 */
void Tc264IrqPort_OnUart0Rx(void);

/** @brief UART1 TX 中断入口 */
void Tc264IrqPort_OnUart1Tx(void);

/** @brief UART1 RX 中断入口 */
void Tc264IrqPort_OnUart1Rx(void);

/** @brief UART2 TX 中断入口 */
void Tc264IrqPort_OnUart2Tx(void);

/** @brief UART2 RX 中断入口 */
void Tc264IrqPort_OnUart2Rx(void);

/** @brief UART3 TX 中断入口 */
void Tc264IrqPort_OnUart3Tx(void);

/** @brief UART3 RX 中断入口 */
void Tc264IrqPort_OnUart3Rx(void);

/** @brief UART0 错误中断入口 */
void Tc264IrqPort_OnUart0Error(void);

/** @brief UART1 错误中断入口 */
void Tc264IrqPort_OnUart1Error(void);

/** @brief UART2 错误中断入口 */
void Tc264IrqPort_OnUart2Error(void);

/** @brief UART3 错误中断入口 */
void Tc264IrqPort_OnUart3Error(void);

#endif /* TC264_IRQ_PORT_H_ */
