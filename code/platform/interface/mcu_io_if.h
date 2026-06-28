#ifndef PLATFORM_INTERFACE_MCU_IO_IF_H_
#define PLATFORM_INTERFACE_MCU_IO_IF_H_

/**
 * @file mcu_io_if.h
 * @brief Platform MCU IO 接口契约。
 *
 * 所有公开函数以 McuIo_ 前缀命名，避免与 Vendor SDK 同名函数冲突。
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================== GPIO ======================== */
#define MCUIO_GPIO_OUTPUT 0
#define MCUIO_GPIO_INPUT  1

typedef struct {
    void     (*init)(uint16_t pin, uint8_t mode);
    void     (*high)(uint16_t pin);
    void     (*low)(uint16_t pin);
    uint8_t  (*read)(uint16_t pin);
} gpio_ops_t;

void McuIo_GpioRegister(const gpio_ops_t *p_ops);
void McuIo_GpioInit(uint16_t pin, uint8_t mode);
void McuIo_GpioHigh(uint16_t pin);
void McuIo_GpioLow(uint16_t pin);
uint8_t McuIo_GpioRead(uint16_t pin);

/* ======================== PWM ======================== */
typedef struct {
    void (*init)(uint16_t ch, uint32_t freq_hz, uint32_t duty);
    void (*set_duty)(uint16_t ch, uint32_t duty);
} pwm_ops_t;

void McuIo_PwmRegister(const pwm_ops_t *p_ops);
void McuIo_PwmInit(uint16_t ch, uint32_t freq_hz, uint32_t duty);
void McuIo_PwmSetDuty(uint16_t ch, uint32_t duty);

/* ======================== UART ======================== */
typedef struct {
    void (*init)(uint16_t ch, uint32_t baud);
} uart_ops_t;

void McuIo_UartRegister(const uart_ops_t *p_ops);
void McuIo_UartInit(uint16_t ch, uint32_t baud);

/* ======================== Encoder ======================== */
typedef struct {
    void    (*init)(uint16_t ch);
    int32_t (*get)(uint16_t ch);
    void    (*clear)(uint16_t ch);
} encoder_ops_t;

void McuIo_EncoderRegister(const encoder_ops_t *p_ops);
void McuIo_EncoderInit(uint16_t ch);
int32_t McuIo_EncoderGet(uint16_t ch);
void McuIo_EncoderClear(uint16_t ch);

/* ======================== PIT ======================== */
typedef struct {
    void (*init)(uint16_t ch, uint32_t period_ms);
    void (*clear_flag)(uint16_t ch);
} pit_ops_t;

void McuIo_PitRegister(const pit_ops_t *p_ops);
void McuIo_PitInit(uint16_t ch, uint32_t period_ms);
void McuIo_PitClearFlag(uint16_t ch);

/* ======================== 诊断 ======================== */
/** @brief MCU IO 全组件已注册返回 1，否则 0。*/
uint8_t McuIo_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_INTERFACE_MCU_IO_IF_H_ */
