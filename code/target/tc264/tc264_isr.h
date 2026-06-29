#ifndef TC264_ISR_H_
#define TC264_ISR_H_

/**
 * @file tc264_isr.h
 * @brief TC264 中断适配入口。
 * @author GS_Mark
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 处理 CCU60 PIT CH0 编码器采样中断 */
void Tc264Isr_Ccu60PitCh0(void);

/** @brief 处理 CCU60 PIT CH1 陀螺仪节拍中断 */
void Tc264Isr_Ccu60PitCh1(void);

/** @brief 处理 ERU CH3/CH7 外部中断 */
void Tc264Isr_ExtiCh3Ch7(void);

/** @brief 处理 DMA CH5 摄像头帧完成中断 */
void Tc264Isr_DmaCh5(void);

/** @brief 处理 UART0 RX 中断 */
void Tc264Isr_Uart0Rx(void);

/** @brief 处理 UART1 RX 中断 */
void Tc264Isr_Uart1Rx(void);

/** @brief 处理 UART3 RX 中断 */
void Tc264Isr_Uart3Rx(void);

/** @brief 处理 UART0 错误中断 */
void Tc264Isr_Uart0Error(void);

/** @brief 处理 UART1 错误中断 */
void Tc264Isr_Uart1Error(void);

/** @brief 处理 UART3 错误中断 */
void Tc264Isr_Uart3Error(void);

#ifdef __cplusplus
}
#endif

#endif /* TC264_ISR_H_ */
