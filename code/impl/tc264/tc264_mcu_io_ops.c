/**
 * @file tc264_mcu_io_ops.c
 * @brief TC264 MCU IO 链接期端口实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件直接实现 SystemPort_* 和 McuIo_* 符号，把中性端口契约绑定到 TC264 Vendor SDK。
 */

#include "platform/interface/mcu_io_if.h"
#include "smartcar_board_resources.h"
#include "platform/system/system_port.h"
#include "tc264_board_map.h"
#include "zf_common_headfile.h"

/**
 * @brief 初始化 TC264 系统时钟。
 *
 * @return void。
 */
void SystemPort_ClockInit(void)
{
    clock_init();
}

/**
 * @brief 初始化调试串口。
 *
 * @return void。
 */
void SystemPort_DebugInit(void)
{
    debug_init();
}

/**
 * @brief 等待多核启动同步。
 *
 * @return void。
 */
void SystemPort_CoreSync(void)
{
    cpu_wait_event_ready();
}

/**
 * @brief 关闭全局中断并返回恢复状态。
 *
 * @return Vendor 全局中断恢复状态。
 */
uint32_t SystemPort_IrqGlobalDisable(void)
{
    return interrupt_global_disable();
}

/**
 * @brief 恢复全局中断状态。
 *
 * @param[in] state SystemPort_IrqGlobalDisable 返回的状态。
 * @return void。
 */
void SystemPort_IrqGlobalRestore(uint32_t state)
{
    interrupt_global_enable(state);
}

/**
 * @brief 设置全局中断控制状态。
 *
 * @param[in] state Vendor 全局中断控制参数。
 * @return void。
 */
void SystemPort_IrqGlobalCtrl(uint8_t state)
{
    interrupt_global_enable(state);
}

/**
 * @brief 初始化 GPIO。
 *
 * Steps:
 *   1. 校验产品 GPIO 资源 ID。
 *   2. 按输出或输入模式调用 TC264 GPIO 初始化。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @param[in] mode MCUIO_GPIO_OUTPUT 或输入模式。
 * @return void。
 */
void McuIo_GpioInit(uint16_t pin, uint8_t mode)
{
    if (pin >= SMARTCAR_GPIO_ID_MAX)
    {
        return;
    }

    if (mode == MCUIO_GPIO_OUTPUT)
    {
        gpio_init(g_tc264_gpio_map[pin], GPO, GPIO_LOW, GPO_PUSH_PULL);
    }
    else
    {
        gpio_init(g_tc264_gpio_map[pin], GPI, GPIO_HIGH, GPI_PULL_UP);
    }
}

/**
 * @brief 拉高 GPIO。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @return void。
 */
void McuIo_GpioHigh(uint16_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_high(g_tc264_gpio_map[pin]);
    }
}

/**
 * @brief 拉低 GPIO。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @return void。
 */
void McuIo_GpioLow(uint16_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_low(g_tc264_gpio_map[pin]);
    }
}

/**
 * @brief 初始化 PWM 通道。
 *
 * @param[in] channel 产品 PWM 资源 ID。
 * @param[in] freq_hz PWM 频率，单位 Hz。
 * @param[in] duty 初始占空比。
 * @return void。
 */
void McuIo_PwmInit(uint16_t channel, uint32_t freq_hz, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_init(g_tc264_pwm_map[channel], freq_hz, duty);
    }
}

/**
 * @brief 设置 PWM 占空比。
 *
 * @param[in] channel 产品 PWM 资源 ID。
 * @param[in] duty 目标占空比。
 * @return void。
 */
void McuIo_PwmSetDuty(uint16_t channel, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_set_duty(g_tc264_pwm_map[channel], duty);
    }
}

/**
 * @brief 初始化 UART。
 *
 * Steps:
 *   1. 校验产品 UART 资源 ID。
 *   2. 从映射表取出 UART 编号和 TX/RX 引脚。
 *   3. 调用 Vendor UART 初始化。
 *
 * @param[in] channel 产品 UART 资源 ID。
 * @param[in] baud 波特率。
 * @return void。
 */
void McuIo_UartInit(uint16_t channel, uint32_t baud)
{
    if (channel >= SMARTCAR_UART_ID_MAX)
    {
        return;
    }

    uart_init(g_tc264_uart_map[channel].uart,
              baud,
              g_tc264_uart_map[channel].tx,
              g_tc264_uart_map[channel].rx);
}

/**
 * @brief 初始化编码器通道。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return void。
 */
void McuIo_EncoderInit(uint16_t channel)
{
    if (channel >= SMARTCAR_ENCODER_ID_MAX)
    {
        return;
    }

    encoder_dir_init(g_tc264_encoder_map[channel].timer,
                     g_tc264_encoder_map[channel].ch1_pin,
                     g_tc264_encoder_map[channel].ch2_pin);
}

/**
 * @brief 读取编码器计数。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return 编码器计数；资源 ID 无效时返回 0。
 */
int32_t McuIo_EncoderGet(uint16_t channel)
{
    int32_t count = 0;

    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        count = encoder_get_count(g_tc264_encoder_map[channel].timer);
    }

    return count;
}

/**
 * @brief 清零编码器计数。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return void。
 */
void McuIo_EncoderClear(uint16_t channel)
{
    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        encoder_clear_count(g_tc264_encoder_map[channel].timer);
    }
}

/**
 * @brief 初始化 PIT 周期中断。
 *
 * @param[in] channel 产品 PIT 资源 ID。
 * @param[in] period_ms 周期，单位 ms。
 * @return void。
 */
void McuIo_PitInit(uint16_t channel, uint32_t period_ms)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_ms_init(g_tc264_pit_map[channel], period_ms);
    }
}

/**
 * @brief 清除 PIT 中断标志。
 *
 * @param[in] channel 产品 PIT 资源 ID。
 * @return void。
 */
void McuIo_PitClearFlag(uint16_t channel)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_clear_flag(g_tc264_pit_map[channel]);
    }
}
