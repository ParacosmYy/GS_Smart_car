/**
 * @file tc264_mcu_io_ops.c
 * @brief TC264 MCU IO 链接期端口实现。
 */

#include "platform/interface/mcu_io_if.h"
#include "platform/system/system_port.h"
#include "system/board/smartcar_board_resources.h"
#include "tc264_board_map.h"
#include "zf_common_headfile.h"

void SystemPort_ClockInit(void)
{
    clock_init();
}

void SystemPort_DebugInit(void)
{
    debug_init();
}

void SystemPort_CoreSync(void)
{
    cpu_wait_event_ready();
}

uint32_t SystemPort_IrqGlobalDisable(void)
{
    return interrupt_global_disable();
}

void SystemPort_IrqGlobalRestore(uint32_t state)
{
    interrupt_global_enable(state);
}

void SystemPort_IrqGlobalCtrl(uint8_t state)
{
    interrupt_global_enable(state);
}

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

void McuIo_GpioHigh(uint16_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_high(g_tc264_gpio_map[pin]);
    }
}

void McuIo_GpioLow(uint16_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_low(g_tc264_gpio_map[pin]);
    }
}

uint8_t McuIo_GpioRead(uint16_t pin)
{
    uint8_t level = 0U;

    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        level = (uint8_t)gpio_get_level(g_tc264_gpio_map[pin]);
    }

    return level;
}

void McuIo_PwmInit(uint16_t channel, uint32_t freq_hz, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_init(g_tc264_pwm_map[channel], freq_hz, duty);
    }
}

void McuIo_PwmSetDuty(uint16_t channel, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_set_duty(g_tc264_pwm_map[channel], duty);
    }
}

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

int32_t McuIo_EncoderGet(uint16_t channel)
{
    int32_t count = 0;

    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        count = encoder_get_count(g_tc264_encoder_map[channel].timer);
    }

    return count;
}

void McuIo_EncoderClear(uint16_t channel)
{
    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        encoder_clear_count(g_tc264_encoder_map[channel].timer);
    }
}

void McuIo_PitInit(uint16_t channel, uint32_t period_ms)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_ms_init(g_tc264_pit_map[channel], period_ms);
    }
}

void McuIo_PitClearFlag(uint16_t channel)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_clear_flag(g_tc264_pit_map[channel]);
    }
}
