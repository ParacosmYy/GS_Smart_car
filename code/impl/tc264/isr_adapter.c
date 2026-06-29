/**
 * @file isr_adapter.c
 * @brief TC264 ISR adapter 硬件清标志和事实上报实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件是 TC264 Vendor ISR 和 System IRQ router 之间的薄适配层。
 * 每个入口完成硬件清标志、采样或 Vendor 回调，然后返回 IRQ_FACT_* 给 System。
 */

#include "isr_adapter.h"

#include "platform/interface/mcu_io_if.h"
#include "smartcar_board_resources.h"
#include "platform/system/system_port.h"
#include "zf_common_headfile.h"

#define ENCODER_WINDOW_SAMPLES 5

typedef struct
{
    volatile int left_sum;
    volatile int right_sum;
    volatile int samples;
    volatile uint8_t ready;
} encoder_window_t;

static encoder_window_t s_encoder_window = {0, 0, 0, 0U};

/**
 * @brief 清理 PIT 中断前置状态。
 *
 * Steps:
 *   1. 打开全局中断控制。
 *   2. 清除指定 PIT 通道的中断标志。
 *
 * @param[in] pit 产品 PIT 资源 ID。
 * @return void。
 */
static void prepare_pit(uint16_t pit)
{
    SystemPort_IrqGlobalCtrl(0);
    McuIo_PitClearFlag(pit);
}

/**
 * @brief 采样左右编码器并累加到窗口。
 *
 * Steps:
 *   1. 读取左右编码器计数并累加。
 *   2. 增加窗口采样次数。
 *   3. 清零硬件编码器计数，准备下一次采样。
 *
 * @return void。
 */
static void sample_encoder(void)
{
    s_encoder_window.left_sum += (int)McuIo_EncoderGet(SMARTCAR_ENCODER_LEFT);
    s_encoder_window.right_sum += (int)McuIo_EncoderGet(SMARTCAR_ENCODER_RIGHT);
    s_encoder_window.samples++;

    McuIo_EncoderClear(SMARTCAR_ENCODER_LEFT);
    McuIo_EncoderClear(SMARTCAR_ENCODER_RIGHT);
}

/**
 * @brief 处理 CCU60 PIT CH0 编码器采样中断。
 *
 * Steps:
 *   1. 清除 PIT 标志。
 *   2. 累加一次左右编码器采样。
 *   3. 满足窗口采样数时返回 IRQ_FACT_ENCODER_WINDOW。
 *
 * @return 编码器窗口就绪事实；未就绪返回 IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Ccu60PitCh0(void)
{
    irq_fact_t fact = IRQ_FACT_NONE;

    prepare_pit(SMARTCAR_PIT_ENCODER_SAMPLE);
    sample_encoder();

    if ((s_encoder_window.samples >= ENCODER_WINDOW_SAMPLES) && (s_encoder_window.ready == 0U))
    {
        s_encoder_window.ready = 1U;
        fact = IRQ_FACT_ENCODER_WINDOW;
    }

    return fact;
}

/**
 * @brief 处理 CCU60 PIT CH1 陀螺仪节拍中断。
 *
 * Steps:
 *   1. 清除 PIT 标志。
 *   2. 返回系统 10ms tick 事实。
 *
 * @return IRQ_FACT_GYRO_TICK。
 */
irq_fact_t IsrAdapter_Ccu60PitCh1(void)
{
    prepare_pit(SMARTCAR_PIT_GYRO_TICK);
    return IRQ_FACT_GYRO_TICK;
}

/**
 * @brief 处理 ERU CH3/CH7 外部中断。
 *
 * Steps:
 *   1. 检查并清除摄像头 VSYNC 标志。
 *   2. 调用 Vendor 摄像头 VSYNC handler。
 *   3. 清除 CH7 预留标志。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_ExtiCh3Ch7(void)
{
    SystemPort_IrqGlobalCtrl(0);

    if (exti_flag_get(ERU_CH3_REQ6_P02_0))
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();
    }

    if (exti_flag_get(ERU_CH7_REQ16_P15_1))
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);
    }

    return IRQ_FACT_NONE;
}

/**
 * @brief 处理 DMA CH5 摄像头帧完成中断。
 *
 * Steps:
 *   1. 调用 Vendor 摄像头 DMA handler。
 *   2. 返回摄像头帧完成事实。
 *
 * @return IRQ_FACT_CAMERA_FRAME。
 */
irq_fact_t IsrAdapter_DmaCh5(void)
{
    SystemPort_IrqGlobalCtrl(0);
    camera_dma_handler();
    return IRQ_FACT_CAMERA_FRAME;
}

/**
 * @brief 处理 UART0 RX 中断。
 *
 * Steps:
 *   1. 打开全局中断控制。
 *   2. 在调试串口中断开启时转发给 Vendor 调试 handler。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart0Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);

#if DEBUG_UART_USE_INTERRUPT
    debug_interrupr_handler();
#endif

    return IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART1 RX 中断。
 *
 * Steps:
 *   1. 转发到摄像头配置串口 Vendor handler。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart1Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);
    camera_uart_handler();
    return IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART3 RX 中断。
 *
 * Steps:
 *   1. 转发到无线串口 Vendor handler。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart3Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);
    wireless_module_uart_handler();
    return IRQ_FACT_NONE;
}

/**
 * @brief 处理 ASCLIN UART 错误中断。
 *
 * Steps:
 *   1. 打开全局中断控制。
 *   2. 调用 iLLD 错误 ISR。
 *
 * @param[in,out] p_handle ASCLIN 句柄。
 * @return IRQ_FACT_NONE。
 */
static irq_fact_t uart_error(IfxAsclin_Asc *p_handle)
{
    SystemPort_IrqGlobalCtrl(0);
    IfxAsclin_Asc_isrError(p_handle);
    return IRQ_FACT_NONE;
}

/**
 * @brief 处理 UART0 错误中断。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart0Error(void)
{
    return uart_error(&uart0_handle);
}

/**
 * @brief 处理 UART1 错误中断。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart1Error(void)
{
    return uart_error(&uart1_handle);
}

/**
 * @brief 处理 UART3 错误中断。
 *
 * @return IRQ_FACT_NONE。
 */
irq_fact_t IsrAdapter_Uart3Error(void)
{
    return uart_error(&uart3_handle);
}

/**
 * @brief 读取并清零编码器窗口采样。
 *
 * Steps:
 *   1. 校验输出指针。
 *   2. 关闭全局中断，复制窗口累加值。
 *   3. 清零窗口状态并恢复中断。
 *
 * @param[out] p_left_sum 左编码器窗口累加输出。
 * @param[out] p_right_sum 右编码器窗口累加输出。
 * @param[out] p_sample_count 窗口采样次数输出。
 * @return void。
 */
void EncoderSample_TakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count)
{
    uint32_t irq_state;

    if ((p_left_sum == 0) || (p_right_sum == 0) || (p_sample_count == 0))
    {
        return;
    }

    irq_state = SystemPort_IrqGlobalDisable();

    *p_left_sum = s_encoder_window.left_sum;
    *p_right_sum = s_encoder_window.right_sum;
    if (s_encoder_window.samples > 0)
    {
        *p_sample_count = s_encoder_window.samples;
    }
    else
    {
        *p_sample_count = ENCODER_WINDOW_SAMPLES;
    }

    s_encoder_window.left_sum = 0;
    s_encoder_window.right_sum = 0;
    s_encoder_window.samples = 0;
    s_encoder_window.ready = 0U;

    SystemPort_IrqGlobalRestore(irq_state);
}
