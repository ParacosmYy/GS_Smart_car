#ifndef TARGET_TC264_OPS_TC264_IRQ_H_
#define TARGET_TC264_OPS_TC264_IRQ_H_

/**
 * @file ops_tc264_irq.h
 * @brief TC264 interrupt adapter entry points.
 * @author GS_Mark
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 处理编码器采样 PIT 中断 */
void TargetIrq_EncoderPit(void);

/** @brief 处理陀螺仪节拍 PIT 中断 */
void TargetIrq_GyroPit(void);

/** @brief 处理摄像头场同步中断 */
void TargetIrq_CameraVsync(void);

/** @brief 处理摄像头 DMA 帧完成中断 */
void TargetIrq_CameraDma(void);

/** @brief 处理 UART0 RX 中断 */
void TargetIrq_Uart0Rx(void);

/** @brief 处理 UART1 RX 中断 */
void TargetIrq_Uart1Rx(void);

/** @brief 处理 UART3 RX 中断 */
void TargetIrq_Uart3Rx(void);

/** @brief 处理 UART0 错误中断 */
void TargetIrq_Uart0Error(void);

/** @brief 处理 UART1 错误中断 */
void TargetIrq_Uart1Error(void);

/** @brief 处理 UART3 错误中断 */
void TargetIrq_Uart3Error(void);

#ifdef __cplusplus
}
#endif

#endif /* TARGET_TC264_OPS_TC264_IRQ_H_ */
