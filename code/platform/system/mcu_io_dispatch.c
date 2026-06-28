/**
 * @file mcu_io_dispatch.c
 * @brief MCU IO dispatch + 就绪诊断。
 */

#include "platform/interface/mcu_io_if.h"

static const gpio_ops_t    *s_gpio    = 0;
static const pwm_ops_t     *s_pwm     = 0;
static const uart_ops_t    *s_uart    = 0;
static const encoder_ops_t *s_encoder = 0;
static const pit_ops_t     *s_pit     = 0;

void McuIo_GpioRegister(const gpio_ops_t *p)    { s_gpio = p; }
void McuIo_PwmRegister(const pwm_ops_t *p)      { s_pwm = p; }
void McuIo_UartRegister(const uart_ops_t *p)    { s_uart = p; }
void McuIo_EncoderRegister(const encoder_ops_t *p) { s_encoder = p; }
void McuIo_PitRegister(const pit_ops_t *p)      { s_pit = p; }

void McuIo_GpioInit(uint16_t p, uint8_t m)   { if (s_gpio && s_gpio->init) s_gpio->init(p, m); }
void McuIo_GpioHigh(uint16_t p)              { if (s_gpio && s_gpio->high) s_gpio->high(p); }
void McuIo_GpioLow(uint16_t p)               { if (s_gpio && s_gpio->low) s_gpio->low(p); }
uint8_t McuIo_GpioRead(uint16_t p)           { return (s_gpio && s_gpio->read) ? s_gpio->read(p) : 0U; }
void McuIo_PwmInit(uint16_t c, uint32_t f, uint32_t d)  { if (s_pwm && s_pwm->init) s_pwm->init(c, f, d); }
void McuIo_PwmSetDuty(uint16_t c, uint32_t d)            { if (s_pwm && s_pwm->set_duty) s_pwm->set_duty(c, d); }
void McuIo_UartInit(uint16_t c, uint32_t b)  { if (s_uart && s_uart->init) s_uart->init(c, b); }
void McuIo_EncoderInit(uint16_t c)           { if (s_encoder && s_encoder->init) s_encoder->init(c); }
int32_t McuIo_EncoderGet(uint16_t c)         { return (s_encoder && s_encoder->get) ? s_encoder->get(c) : 0; }
void McuIo_EncoderClear(uint16_t c)          { if (s_encoder && s_encoder->clear) s_encoder->clear(c); }
void McuIo_PitInit(uint16_t c, uint32_t m)   { if (s_pit && s_pit->init) s_pit->init(c, m); }
void McuIo_PitClearFlag(uint16_t c)          { if (s_pit && s_pit->clear_flag) s_pit->clear_flag(c); }

/* ── 诊断 ────────────────────────────────────── */
uint8_t McuIo_IsReady(void)
{
    return (s_gpio != 0) && (s_pwm != 0) && (s_uart != 0)
        && (s_encoder != 0) && (s_pit != 0) ? 1U : 0U;
}
