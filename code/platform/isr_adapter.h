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
 * @brief TC264 ISR adapter entry points
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

//******************************* Declaring *********************************//
/**
 * @brief Handle CCU60 PIT channel 0 interrupt.
 */
void IsrAdapter_Ccu60PitCh0(void);

/**
 * @brief Handle CCU60 PIT channel 1 interrupt.
 */
void IsrAdapter_Ccu60PitCh1(void);

/**
 * @brief Handle CCU61 PIT channel 0 interrupt.
 */
void IsrAdapter_Ccu61PitCh0(void);

/**
 * @brief Handle CCU61 PIT channel 1 interrupt.
 */
void IsrAdapter_Ccu61PitCh1(void);

/**
 * @brief Handle ERU channel 0/4 interrupt.
 */
void IsrAdapter_ExtiCh0Ch4(void);

/**
 * @brief Handle ERU channel 1/5 interrupt.
 */
void IsrAdapter_ExtiCh1Ch5(void);

/**
 * @brief Handle ERU channel 3/7 interrupt.
 */
void IsrAdapter_ExtiCh3Ch7(void);

/**
 * @brief Handle DMA channel 5 interrupt.
 */
void IsrAdapter_DmaCh5(void);

/**
 * @brief Handle UART0 TX interrupt.
 */
void IsrAdapter_Uart0Tx(void);

/**
 * @brief Handle UART0 RX interrupt.
 */
void IsrAdapter_Uart0Rx(void);

/**
 * @brief Handle UART1 TX interrupt.
 */
void IsrAdapter_Uart1Tx(void);

/**
 * @brief Handle UART1 RX interrupt.
 */
void IsrAdapter_Uart1Rx(void);

/**
 * @brief Handle UART2 TX interrupt.
 */
void IsrAdapter_Uart2Tx(void);

/**
 * @brief Handle UART2 RX interrupt.
 */
void IsrAdapter_Uart2Rx(void);

/**
 * @brief Handle UART3 TX interrupt.
 */
void IsrAdapter_Uart3Tx(void);

/**
 * @brief Handle UART3 RX interrupt.
 */
void IsrAdapter_Uart3Rx(void);

/**
 * @brief Handle UART0 error interrupt.
 */
void IsrAdapter_Uart0Error(void);

/**
 * @brief Handle UART1 error interrupt.
 */
void IsrAdapter_Uart1Error(void);

/**
 * @brief Handle UART2 error interrupt.
 */
void IsrAdapter_Uart2Error(void);

/**
 * @brief Handle UART3 error interrupt.
 */
void IsrAdapter_Uart3Error(void);

/**
 * @brief Take and reset encoder accumulation snapshot.
 */
void IsrAdapter_TakeEncoderSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#ifdef __cplusplus
}
#endif

#endif /* ISR_ADAPTER_H_ */
