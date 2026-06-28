/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file smartcar_isr_bridge.c
 *
 * @par dependencies
 * - smartcar_isr_bridge.h
 * - config.h
 * - data.h
 * - event.h
 * - isr_adapter.h
 * - scheduler.h
 *
 * @author GS_Mark
 *
 * @brief App-level ISR bridge implementation.
 *
 * Processing flow:
 * TC264 ISR entries call this bridge. The bridge delegates hardware flag
 * handling to IsrAdapter, then translates platform-neutral ISR facts into
 * scheduler events and app time base updates.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "smartcar_isr_bridge.h"

#include "config.h"
#include "data.h"
#include "event.h"
#include "isr_adapter.h"
#include "scheduler.h"
//******************************** Includes *********************************//

//******************************** Implement ********************************//
/**
 * @brief 桥接 CCU60 PIT 通道 0 中断。
 *
 * 处理步骤：
 *  1. 委托 ISR adapter 清标志并累加编码器采样窗口。
 *  2. 当平台层报告测速窗口完成时，发布编码器服务事件。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarIsrBridge_Ccu60PitCh0(void)
{
    isr_adapter_event_t events = ISR_ADAPTER_EVT_NONE;

    events = IsrAdapter_Ccu60PitCh0();
    if ((events & ISR_ADAPTER_EVT_ENCODER_WINDOW) != 0U)
    {
        event_set_isr(EVT_ENCODER_50MS);
    }
}

/**
 * @brief 桥接 CCU60 PIT 通道 1 中断。
 *
 * 处理步骤：
 *  1. 委托 ISR adapter 清除 PIT 中断标志。
 *  2. 在 App 调度边界推进系统时间基。
 *  3. 发布陀螺仪 10ms 计数事件。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarIsrBridge_Ccu60PitCh1(void)
{
    isr_adapter_event_t events = ISR_ADAPTER_EVT_NONE;

    events = IsrAdapter_Ccu60PitCh1();
    if ((events & ISR_ADAPTER_EVT_GYRO_TICK) != 0U)
    {
        g_system_ms += PIT_PERIOD_MS;
        pit_ch1_count++;
        event_set_isr(EVT_GYRO_10MS);
    }
}
//******************************** Implement ********************************//
