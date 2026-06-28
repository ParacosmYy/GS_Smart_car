/**
 * @file tc264_mcu_io_ops.c
 * @brief TC264 MCU IO ops 聚合 —— GPIO / PWM / UART / Encoder / PIT / System。
 */

#include "platform/interface/mcu_io_if.h"
#include "platform/system/system_port.h"
#include "tc264_board_map.h"
#include "zf_common_headfile.h"

/* ======================== System ======================== */
void SystemPort_ClockInit(void)   { clock_init(); }
void SystemPort_DebugInit(void)   { debug_init(); }
void SystemPort_CoreSync(void)    { cpu_wait_event_ready(); }
uint32_t SystemPort_IrqGlobalDisable(void)  { return interrupt_global_disable(); }
void SystemPort_IrqGlobalRestore(uint32_t s) { interrupt_global_enable(s); }
void SystemPort_IrqGlobalCtrl(uint8_t e)    { interrupt_global_enable(e); }

/* ======================== GPIO ======================== */
static void tc264_gpio_init(uint16_t pin, uint8_t mode)
{
    if (pin >= SMARTCAR_GPIO_ID_MAX) { return; }
    if (mode == MCUIO_GPIO_OUTPUT)
        gpio_init(g_tc264_gpio_map[pin], GPO, GPIO_LOW, GPO_PUSH_PULL);
    else
        gpio_init(g_tc264_gpio_map[pin], GPI, GPIO_HIGH, GPI_PULL_UP);
}
static void tc264_gpio_high(uint16_t pin) { if (pin < SMARTCAR_GPIO_ID_MAX) gpio_high(g_tc264_gpio_map[pin]); }
static void tc264_gpio_low(uint16_t pin)  { if (pin < SMARTCAR_GPIO_ID_MAX) gpio_low(g_tc264_gpio_map[pin]); }
static uint8_t tc264_gpio_read(uint16_t pin) { return (pin < SMARTCAR_GPIO_ID_MAX) ? (uint8_t)gpio_get_level(g_tc264_gpio_map[pin]) : 0U; }
const gpio_ops_t g_tc264_gpio_ops = { .init = tc264_gpio_init, .high = tc264_gpio_high, .low = tc264_gpio_low, .read = tc264_gpio_read };

/* ======================== PWM ======================== */
static void tc264_pwm_init(uint16_t ch, uint32_t f, uint32_t d) { if (ch < SMARTCAR_PWM_ID_MAX) pwm_init(g_tc264_pwm_map[ch], f, d); }
static void tc264_pwm_set_duty(uint16_t ch, uint32_t d) { if (ch < SMARTCAR_PWM_ID_MAX) pwm_set_duty(g_tc264_pwm_map[ch], d); }
const pwm_ops_t g_tc264_pwm_ops = { .init = tc264_pwm_init, .set_duty = tc264_pwm_set_duty };

/* ======================== UART ======================== */
static void tc264_uart_init(uint16_t ch, uint32_t b)
{
    if (ch >= SMARTCAR_UART_ID_MAX) { return; }
    uart_init(g_tc264_uart_map[ch].uart, b, g_tc264_uart_map[ch].tx, g_tc264_uart_map[ch].rx);
}
const uart_ops_t g_tc264_uart_ops = { .init = tc264_uart_init };

/* ======================== Encoder ======================== */
static void tc264_encoder_init(uint16_t ch)
{
    if (ch >= SMARTCAR_ENCODER_ID_MAX) { return; }
    encoder_dir_init(g_tc264_encoder_map[ch].timer, g_tc264_encoder_map[ch].ch1_pin, g_tc264_encoder_map[ch].ch2_pin);
}
static int32_t tc264_encoder_get(uint16_t ch) { return (ch < SMARTCAR_ENCODER_ID_MAX) ? encoder_get_count(g_tc264_encoder_map[ch].timer) : 0; }
static void tc264_encoder_clear(uint16_t ch) { if (ch < SMARTCAR_ENCODER_ID_MAX) encoder_clear_count(g_tc264_encoder_map[ch].timer); }
const encoder_ops_t g_tc264_encoder_ops = { .init = tc264_encoder_init, .get = tc264_encoder_get, .clear = tc264_encoder_clear };

/* ======================== PIT ======================== */
static void tc264_pit_init(uint16_t ch, uint32_t ms) { if (ch < SMARTCAR_PIT_ID_MAX) pit_ms_init(g_tc264_pit_map[ch], ms); }
static void tc264_pit_clear_flag(uint16_t ch) { if (ch < SMARTCAR_PIT_ID_MAX) pit_clear_flag(g_tc264_pit_map[ch]); }
const pit_ops_t g_tc264_pit_ops = { .init = tc264_pit_init, .clear_flag = tc264_pit_clear_flag };
