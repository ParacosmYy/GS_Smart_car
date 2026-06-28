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
 * @brief Handle CCU60 PIT channel 0 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear PIT flag.
 *  3. Accumulate encoder counts and clear hardware counters.
 *  4. Publish encoder window event after enough samples.
 *
 * @return void : None.
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
 * @brief Handle CCU60 PIT channel 1 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear PIT flag.
 *  3. Advance system tick and publish gyro tick event.
 *
 * @return void : None.
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
 * @brief Handle CCU61 PIT channel 0 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear PIT flag.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Ccu61PitCh0(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_2);
}

/**
 * @brief Handle CCU61 PIT channel 1 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear PIT flag.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Ccu61PitCh1(void)
{
    pal_irq_global_ctrl(0);
    pal_pit_clear_flag(PAL_CH_PIT_3);
}

/**
 * @brief Handle ERU channel 0/4 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear any active ERU source flags.
 *
 * @return void : None.
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
 * @brief Handle ERU channel 1/5 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear any active ERU source flags.
 *
 * @return void : None.
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
 * @brief Handle ERU channel 3/7 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Clear camera VSYNC source and call camera callback.
 *  3. Clear spare ERU source if active.
 *
 * @return void : None.
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
 * @brief Handle DMA channel 5 interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Call camera DMA completion callback.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_DmaCh5(void)
{
    pal_irq_global_ctrl(0);
    camera_dma_handler();
}

/**
 * @brief Handle UART0 TX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart0Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief Handle UART0 RX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch debug UART receive handler when enabled.
 *
 * @return void : None.
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
 * @brief Handle UART1 TX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart1Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief Handle UART1 RX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch camera UART callback.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart1Rx(void)
{
    pal_irq_global_ctrl(0);
    camera_uart_handler();
}

/**
 * @brief Handle UART2 TX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart2Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief Handle UART2 RX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch wireless UART callback.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart2Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_wireless_rx_handler();
}

/**
 * @brief Handle UART3 TX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart3Tx(void)
{
    pal_irq_global_ctrl(0);
}

/**
 * @brief Handle UART3 RX interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch GNSS UART callback.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart3Rx(void)
{
    pal_irq_global_ctrl(0);
    pal_gnss_rx_callback();
}

/**
 * @brief Handle UART0 error interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch ASCLIN error handler.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart0Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart0_handle);
}

/**
 * @brief Handle UART1 error interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch ASCLIN error handler.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart1Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart1_handle);
}

/**
 * @brief Handle UART2 error interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch ASCLIN error handler.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart2Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart2_handle);
}

/**
 * @brief Handle UART3 error interrupt.
 *
 * Steps:
 *  1. Enable nested interrupt handling for TC264 compatibility.
 *  2. Dispatch ASCLIN error handler.
 *
 * @return void : None.
 *
 * */
void IsrAdapter_Uart3Error(void)
{
    pal_irq_global_ctrl(0);
    IfxAsclin_Asc_isrError(&uart3_handle);
}

/**
 * @brief Take and reset encoder accumulation snapshot.
 *
 * Steps:
 *  1. Enter the shortest possible global interrupt critical section.
 *  2. Copy encoder sums and sample count to caller buffers.
 *  3. Reset the adapter-owned accumulators for the next window.
 *
 * @param[out] p_left_sum     : Left encoder accumulated count.
 * @param[out] p_right_sum    : Right encoder accumulated count.
 * @param[out] p_sample_count : Number of accumulated samples.
 *
 * @return void : None.
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
