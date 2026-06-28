/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file isr_adapter.c
 *
 * @par dependencies
 * - isr_adapter.h
 * - platform.h
 * - zf_common_headfile.h
 *
 * @author GS_Mark
 *
 * @brief TC264 ISR adapter implementation.
 *
 * Processing flow:
 * System IRQ router calls this adapter after user/isr.c identifies the source.
 * The adapter clears hardware flags and performs bounded integer ISR work.
 * Scheduler event publication is owned by SmartcarIrqRouter.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "isr_adapter.h"

#include "platform.h"
#include "zf_common_headfile.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define ISR_ADAPTER_ENCODER_WINDOW_SAMPLES   (5)
//******************************** Defines **********************************//

//******************************** Types ************************************//
typedef struct
{
    volatile int     left_speed_sum;
    volatile int     right_speed_sum;
    volatile int     sample_count;
    volatile uint8_t is_window_ready;
} isr_adapter_encoder_handler_t;

typedef struct
{
    int left_sum;
    int right_sum;
    int sample_count;
} isr_adapter_encoder_snapshot_t;
//******************************** Types ************************************//

//******************************** Variables ********************************//
static isr_adapter_encoder_handler_t s_encoder_handler =
{
    0,
    0,
    0,
    0U
};
//******************************** Variables ********************************//

//******************************** Declaring ********************************//
static void IsrAdapter_PreparePitChannel(pal_ch_t pit_channel);
static void IsrAdapter_EncoderHandlerAccumulate(isr_adapter_encoder_handler_t *p_handler);
static smartcar_irq_fact_t IsrAdapter_EncoderHandlerCheckWindow(isr_adapter_encoder_handler_t *p_handler);
static void IsrAdapter_EncoderHandlerTakeSnapshot(isr_adapter_encoder_handler_t *p_handler,
                                                  isr_adapter_encoder_snapshot_t *p_snapshot);
//******************************** Declaring ********************************//

//******************************** Implement ********************************//
/**
 * @brief 准备 PIT 中断处理上下文。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除指定 PIT 通道的中断标志。
 *
 * @param[in] pit_channel : PIT 逻辑通道。
 *
 * @return void : 无返回值。
 *
 * */
static void IsrAdapter_PreparePitChannel(pal_ch_t pit_channel)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(pit_channel);
}

/**
 * @brief 累加一次编码器测速采样。
 *
 * 处理步骤：
 *  1. 读取左右编码器当前计数。
 *  2. 将计数累加到测速窗口缓存。
 *  3. 清零硬件编码器计数器，准备下一次 10ms 采样。
 *
 * @return void : 无返回值。
 *
 * */
static void IsrAdapter_EncoderHandlerAccumulate(isr_adapter_encoder_handler_t *p_handler)
{
    int left_encoder_count = 0;
    int right_encoder_count = 0;

    if (p_handler == 0)
    {
        return;
    }

    left_encoder_count = (int)pal_encoder_get(PAL_CH_ENCODER_L);
    right_encoder_count = (int)pal_encoder_get(PAL_CH_ENCODER_R);

    p_handler->left_speed_sum += left_encoder_count;
    p_handler->right_speed_sum += right_encoder_count;
    p_handler->sample_count++;

    pal_encoder_clear(PAL_CH_ENCODER_L);
    pal_encoder_clear(PAL_CH_ENCODER_R);
}

/**
 * @brief 检查编码器测速窗口是否已完成。
 *
 * 处理步骤：
 *  1. 判断当前采样数是否达到测速窗口大小。
 *  2. 每个窗口只上报一次完成事件，直到主循环取走快照后重新打开。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
static smartcar_irq_fact_t IsrAdapter_EncoderHandlerCheckWindow(isr_adapter_encoder_handler_t *p_handler)
{
    smartcar_irq_fact_t events = SMARTCAR_IRQ_FACT_NONE;

    if (p_handler == 0)
    {
        return events;
    }

    if ((p_handler->sample_count >= ISR_ADAPTER_ENCODER_WINDOW_SAMPLES)
        && (p_handler->is_window_ready == 0U))
    {
        p_handler->is_window_ready = 1U;
        events |= SMARTCAR_IRQ_FACT_ENCODER_WINDOW;
    }

    return events;
}

/**
 * @brief 复制并复位编码器 Handler 快照。
 *
 * 处理步骤：
 *  1. 复制当前测速窗口累加值。
 *  2. 清零 Handler 内部状态，打开下一轮窗口上报。
 *
 * @param[in,out] p_handler      : 编码器 ISR Handler 上下文。
 * @param[out]    p_left_sum     : 左编码器累加值。
 * @param[out]    p_right_sum    : 右编码器累加值。
 * @param[out]    p_sample_count : 窗口内采样次数。
 *
 * @return void : 无返回值。
 *
 * */
static void IsrAdapter_EncoderHandlerTakeSnapshot(isr_adapter_encoder_handler_t *p_handler,
                                                  isr_adapter_encoder_snapshot_t *p_snapshot)
{
    if ((p_handler == 0) || (p_snapshot == 0))
    {
        return;
    }

    p_snapshot->left_sum = p_handler->left_speed_sum;
    p_snapshot->right_sum = p_handler->right_speed_sum;
    p_snapshot->sample_count = p_handler->sample_count;

    p_handler->left_speed_sum = 0;
    p_handler->right_speed_sum = 0;
    p_handler->sample_count = 0;
    p_handler->is_window_ready = 0U;
}

/**
 * @brief 处理 CCU60 PIT 通道 0 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 累加左右编码器计数，并清零硬件编码器计数器。
 *  4. 累计到测速窗口后返回编码器窗口完成事件。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Ccu60PitCh0(void)
{
    smartcar_irq_fact_t events = SMARTCAR_IRQ_FACT_NONE;

    IsrAdapter_PreparePitChannel(PAL_CH_PIT_0);
    IsrAdapter_EncoderHandlerAccumulate(&s_encoder_handler);
    events = IsrAdapter_EncoderHandlerCheckWindow(&s_encoder_handler);
    return events;
}

/**
 * @brief 处理 CCU60 PIT 通道 1 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 返回陀螺仪 10ms tick 事件，由 system IRQ router 发布调度事件。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Ccu60PitCh1(void)
{
    IsrAdapter_PreparePitChannel(PAL_CH_PIT_1);

    return SMARTCAR_IRQ_FACT_GYRO_TICK;
}

/**
 * @brief 处理 CCU61 PIT 通道 0 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 返回空平台事件，保持 adapter 入口签名一致。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Ccu61PitCh0(void)
{
    IsrAdapter_PreparePitChannel(PAL_CH_PIT_2);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 CCU61 PIT 通道 1 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除 PIT 中断标志。
 *  3. 返回空平台事件，保持 adapter 入口签名一致。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Ccu61PitCh1(void)
{
    IsrAdapter_PreparePitChannel(PAL_CH_PIT_3);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 ERU 通道 0/4 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 检查并清除已触发的 ERU 源标志。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_ExtiCh0Ch4(void)
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

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 ERU 通道 1/5 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 检查并清除已触发的 ERU 源标志。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_ExtiCh1Ch5(void)
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

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 ERU 通道 3/7 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 清除摄像头场同步源标志，并调用摄像头采集回调。
 *  3. 如预留 ERU 源被触发，也同步清除。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_ExtiCh3Ch7(void)
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

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 DMA 通道 5 中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 调用摄像头 DMA 完成回调。
 *  3. 返回摄像头帧完成事实，由 system IRQ router 发布调度事件。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_DmaCh5(void)
{
    pal_irq_global_ctrl(0);
    camera_dma_handler();

    return SMARTCAR_IRQ_FACT_CAMERA_FRAME;
}

/**
 * @brief 处理 UART0 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart0Tx(void)
{
    pal_irq_global_ctrl(0);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART0 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 若启用调试串口中断，则分发到调试串口接收处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart0Rx(void)
{
    pal_irq_global_ctrl(0);

#if DEBUG_UART_USE_INTERRUPT
    debug_interrupr_handler();
#endif

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART1 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart1Tx(void)
{
    pal_irq_global_ctrl(0);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART1 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到摄像头配置串口回调。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart1Rx(void)
{
    pal_irq_global_ctrl(0);
    camera_uart_handler();

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART2 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart2Tx(void)
{
    pal_irq_global_ctrl(0);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART2 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到无线串口接收处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart2Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_wireless_rx_handler();

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART3 发送中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart3Tx(void)
{
    pal_irq_global_ctrl(0);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART3 接收中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 GNSS 串口接收回调。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart3Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_gnss_rx_callback();

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART0 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart0Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart0_handle);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART1 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart1Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart1_handle);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART2 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart2Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart2_handle);

    return SMARTCAR_IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART3 错误中断。
 *
 * 处理步骤：
 *  1. 按 TC264 机制重新打开全局中断，允许更高优先级中断响应。
 *  2. 分发到 ASCLIN 错误处理函数。
 *
 * @return smartcar_irq_fact_t : 平台中断事件。
 *
 * */
smartcar_irq_fact_t IsrAdapter_Uart3Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart3_handle);

    return SMARTCAR_IRQ_FACT_NONE;
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
void pal_encoder_take_snapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count)
{
    isr_adapter_encoder_snapshot_t snapshot = {0, 0, 0};
    uint32_t irq_state = 0;

    if ((p_left_sum == 0) || (p_right_sum == 0) || (p_sample_count == 0))
    {
        return;
    }

    irq_state = pal_irq_global_disable();
    IsrAdapter_EncoderHandlerTakeSnapshot(&s_encoder_handler, &snapshot);
    pal_irq_global_restore(irq_state);

    if (snapshot.sample_count <= 0)
    {
        snapshot.sample_count = ISR_ADAPTER_ENCODER_WINDOW_SAMPLES;
    }

    *p_left_sum = snapshot.left_sum;
    *p_right_sum = snapshot.right_sum;
    *p_sample_count = snapshot.sample_count;
}
