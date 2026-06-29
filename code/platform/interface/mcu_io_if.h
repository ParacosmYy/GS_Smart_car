#ifndef PLATFORM_INTERFACE_MCU_IO_IF_H_
#define PLATFORM_INTERFACE_MCU_IO_IF_H_

/**
 * @file mcu_io_if.h
 * @brief Platform MCU IO 接口契约。
 *
 * 所有公开函数以 McuIo_ 前缀命名，避免与 Vendor SDK 同名函数冲突。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MCUIO_GPIO_OUTPUT 0
#define MCUIO_GPIO_INPUT  1

void McuIo_GpioInit(uint16_t pin, uint8_t mode);
void McuIo_GpioHigh(uint16_t pin);
void McuIo_GpioLow(uint16_t pin);

void McuIo_PwmInit(uint16_t ch, uint32_t freq_hz, uint32_t duty);
void McuIo_PwmSetDuty(uint16_t ch, uint32_t duty);

void McuIo_UartInit(uint16_t ch, uint32_t baud);

void McuIo_EncoderInit(uint16_t ch);
int32_t McuIo_EncoderGet(uint16_t ch);
void McuIo_EncoderClear(uint16_t ch);

void McuIo_PitInit(uint16_t ch, uint32_t period_ms);
void McuIo_PitClearFlag(uint16_t ch);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_INTERFACE_MCU_IO_IF_H_ */
