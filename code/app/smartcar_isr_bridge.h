/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file smartcar_isr_bridge.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author GS_Mark
 *
 * @brief App-level ISR bridge interface.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef SMARTCAR_ISR_BRIDGE_H_
#define SMARTCAR_ISR_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Declaring *********************************//
/**
 * @brief 处理 CCU60 PIT 通道 0 中断桥接
 */
void SmartcarIsrBridge_Ccu60PitCh0(void);

/**
 * @brief 处理 CCU60 PIT 通道 1 中断桥接
 */
void SmartcarIsrBridge_Ccu60PitCh1(void);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_ISR_BRIDGE_H_ */
