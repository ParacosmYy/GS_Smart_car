/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file isr_adapter.c
 *
 * @par dependencies
 * - isr_adapter.h
 * - platform.h
 * - event.h
 * - scheduler.h
 * - zf_common_headfile.h
 *
 * @author GS_Mark
 *
 * @brief TC264 ISR adapter implementation.
 *
 * Processing flow:
 * ISR entry functions in user/isr.c call this adapter. The adapter clears
 * hardware flags, performs bounded integer ISR work, and publishes events to
 * the cooperative scheduler.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "isr_adapter.h"

#include "event.h"
#include "platform.h"
#include "scheduler.h"
#include "zf_common_headfile.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define ISR_ADAPTER_ENCODER_WINDOW_SAMPLES   (5)
//******************************** Defines **********************************//

//******************************** Variables ********************************//
static volatile int s_left_speed_sum = 0;
static volatile int s_right_speed_sum = 0;
static volatile int s_sample_count = 0;

extern volatile int pit_ch1_count;
//******************************** Variables ********************************//

//******************************** Implement ********************************//
/**
 * @brief 处理 CCU60 PIT 通道 0 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 累加左右编码器计数，并清零硬件编码器计数器。
 *  4. 累计到测速窗口后发布编码器事件。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Ccu60PitCh0(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_0);

    s_left_speed_sum += pal_encoder_get(PAL_CH_ENCODER_L);
    s_right_speed_sum += pal_encoder_get(PAL_CH_ENCODER_R);
    pal_encoder_clear(PAL_CH_ENCODER_L);
    pal_encoder_clear(PAL_CH_ENCODER_R);
    s_sample_count++;

    if (s_sample_count >= ISR_ADAPTER_ENCODER_WINDOW_SAMPLES)
    {
        event_set_isr(EVT_ENCODER_50MS);
    }
}

/**
 * @brief 处理 CCU60 PIT 通道 1 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 推进系统时间基，并发布陀螺仪 10ms 事件。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Ccu60PitCh1(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_1);

    g_system_ms += 10U;
    pit_ch1_count++;
    event_set_isr(EVT_GYRO_10MS);
}

/**
 * @brief 处理 CCU61 PIT 通道 0 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Ccu61PitCh0(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_2);
}

/**
 * @brief 处理 CCU61 PIT 通道 1 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Ccu61PitCh1(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_3);
}

/**
 * @brief 处理 ERU 通道 0/4 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 检查并清除已触发的 ERU 源标志。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_ExtiCh0Ch4(void)
{
    pal_irq_global_ctrl(0);

    if (exti_flag_get(ERU_CH0_REQ0_P15_4))
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
    }

    if (exti_flag_get(ERU_CH4_REQ13_P15_5))
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);
    }
}

/**
 * @brief 处理 ERU 通道 1/5 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 检查并清除已触发的 ERU 源标志。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_ExtiCh1Ch5(void)
{
    pal_irq_global_ctrl(0);

    if (exti_flag_get(ERU_CH1_REQ10_P14_3))
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);
    }

    if (exti_flag_get(ERU_CH5_REQ1_P15_8))
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);
    }
}

/**
 * @brief 处理 ERU 通道 3/7 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除摄像头场同步源标志，并调用摄像头采集回调。
 *  3. 如预留 ERU 源被触发，也同步清除。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_ExtiCh3Ch7(void)
{
    pal_irq_global_ctrl(0);

    if (exti_flag_get(ERU_CH3_REQ6_P02_0))
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();
    }

    if (exti_flag_get(ERU_CH7_REQ16_P15_1))
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);
    }
}

/**
 * @brief 处理 DMA 通道 5 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 调用摄像头 DMA 完成回调。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_DmaCh5(void)
{
    pal_irq_global_ctrl(0);
    camera_dma_handler();
}

/**
 * @brief 处理 UART0 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart0Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief 处理 UART0 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 若启用调试串口中断，则分发到调试串口接收处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart0Rx(void)
{
    pal_irq_global_ctrl(0);

#if DEBUG_UART_USE_INTERRUPT
    debug_interrupr_handler();
#endif
}

/**
 * @brief 处理 UART1 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart1Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief 处理 UART1 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到摄像头配置串口回调。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart1Rx(void)
{
    pal_irq_global_ctrl(0);
    camera_uart_handler();
}

/**
 * @brief 处理 UART2 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart2Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief 处理 UART2 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到无线串口接收处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart2Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_wireless_rx_handler();
}

/**
 * @brief 处理 UART3 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart3Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief 处理 UART3 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 GNSS 串口接收回调。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart3Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_gnss_rx_callback();
}

/**
 * @brief 处理 UART0 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart0Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart0_handle);
}

/**
 * @brief 处理 UART1 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart1Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart1_handle);
}

/**
 * @brief 处理 UART2 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart2Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart2_handle);
}

/**
 * @brief 处理 UART3 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_Uart3Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart3_handle);
}

/**
 * @brief 获取并清零编码器累加快照。
 *
 * 处理步骤：
 *  1. 进入尽可能短的全局中断临界区。
 *  2. 将编码器累加值和采样次数复制到调用方缓冲区。
 *  3. 清零适配层内部累加器，准备下一轮测速窗口。
 *
 * @param[out] p_left_sum     : 左编码器累加值。
 * @param[out] p_right_sum    : 右编码器累加值。
 * @param[out] p_sample_count : 已累加的采样次数。
 *
 * @return void : 无返回值。
 *
 * */
void IsrAdapter_TakeEncoderSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count)
{
    uint32_t irq_state = 0;

    if ((p_left_sum == 0) || (p_right_sum == 0) || (p_sample_count == 0))
    {
        return;
    }

    irq_state = pal_irq_global_disable();
    *p_left_sum = s_left_speed_sum;
    *p_right_sum = s_right_speed_sum;
    *p_sample_count = s_sample_count;

    s_left_speed_sum = 0;
    s_right_speed_sum = 0;
    s_sample_count = 0;
    pal_irq_global_restore(irq_state);

    if (*p_sample_count <= 0)
    {
        *p_sample_count = ISR_ADAPTER_ENCODER_WINDOW_SAMPLES;
    }
}
