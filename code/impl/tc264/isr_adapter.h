/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file isr_adapter.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author GS_Mark
 *
 * @brief TC264 target ISR adapter entry points.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef ISR_ADAPTER_H_
#define ISR_ADAPTER_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Includes **********************************//
#include <stdint.h>
//******************************* Includes **********************************//

//******************************* Defines ***********************************//
typedef uint32_t isr_adapter_event_t;

#define ISR_ADAPTER_EVT_NONE            (0U)
#define ISR_ADAPTER_EVT_ENCODER_WINDOW  (1U << 0)
#define ISR_ADAPTER_EVT_GYRO_TICK       (1U << 1)
#define ISR_ADAPTER_EVT_CAMERA_FRAME    (1U << 2)
//******************************* Defines ***********************************//

//******************************* Declaring *********************************//
/**
 * @brief 处理 CCU60 PIT 通道 0 中断
 */
isr_adapter_event_t IsrAdapter_Ccu60PitCh0(void);

/**
 * @brief 处理 CCU60 PIT 通道 1 中断
 */
isr_adapter_event_t IsrAdapter_Ccu60PitCh1(void);

/**
 * @brief 处理 CCU61 PIT 通道 0 中断
 */
isr_adapter_event_t IsrAdapter_Ccu61PitCh0(void);

/**
 * @brief 处理 CCU61 PIT 通道 1 中断
 */
isr_adapter_event_t IsrAdapter_Ccu61PitCh1(void);

/**
 * @brief 处理 ERU 通道 0/4 中断
 */
isr_adapter_event_t IsrAdapter_ExtiCh0Ch4(void);

/**
 * @brief 处理 ERU 通道 1/5 中断
 */
isr_adapter_event_t IsrAdapter_ExtiCh1Ch5(void);

/**
 * @brief 处理 ERU 通道 3/7 中断
 */
isr_adapter_event_t IsrAdapter_ExtiCh3Ch7(void);

/**
 * @brief 处理 DMA 通道 5 中断
 */
isr_adapter_event_t IsrAdapter_DmaCh5(void);

/**
 * @brief 处理 UART0 发送中断
 */
isr_adapter_event_t IsrAdapter_Uart0Tx(void);

/**
 * @brief 处理 UART0 接收中断
 */
isr_adapter_event_t IsrAdapter_Uart0Rx(void);

/**
 * @brief 处理 UART1 发送中断
 */
isr_adapter_event_t IsrAdapter_Uart1Tx(void);

/**
 * @brief 处理 UART1 接收中断
 */
isr_adapter_event_t IsrAdapter_Uart1Rx(void);

/**
 * @brief 处理 UART2 发送中断
 */
isr_adapter_event_t IsrAdapter_Uart2Tx(void);

/**
 * @brief 处理 UART2 接收中断
 */
isr_adapter_event_t IsrAdapter_Uart2Rx(void);

/**
 * @brief 处理 UART3 发送中断
 */
isr_adapter_event_t IsrAdapter_Uart3Tx(void);

/**
 * @brief 处理 UART3 接收中断
 */
isr_adapter_event_t IsrAdapter_Uart3Rx(void);

/**
 * @brief 处理 UART0 错误中断
 */
isr_adapter_event_t IsrAdapter_Uart0Error(void);

/**
 * @brief 处理 UART1 错误中断
 */
isr_adapter_event_t IsrAdapter_Uart1Error(void);

/**
 * @brief 处理 UART2 错误中断
 */
isr_adapter_event_t IsrAdapter_Uart2Error(void);

/**
 * @brief 处理 UART3 错误中断
 */
isr_adapter_event_t IsrAdapter_Uart3Error(void);

#ifdef __cplusplus
}
#endif

#endif /* ISR_ADAPTER_H_ */
