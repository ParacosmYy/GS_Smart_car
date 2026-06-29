#ifndef PLATFORM_PORT_IF_H_
#define PLATFORM_PORT_IF_H_

/**
 * @file port_if.h
 * @brief Stable platform port contract for system, MCU IO, and board devices.
 *
 * Upper layers depend on this header only. Each target provides the
 * SystemPort_*, McuIo_*, and Device_* symbols at link time.
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//******************************** System Port *************************************//
void SystemPort_ClockInit(void);
void SystemPort_DebugInit(void);
void SystemPort_CoreSync(void);
uint32_t SystemPort_IrqGlobalDisable(void);
void SystemPort_IrqGlobalRestore(uint32_t state);
void SystemPort_IrqGlobalCtrl(uint8_t state);

//******************************** MCU IO Port *************************************//
#define MCUIO_GPIO_OUTPUT 0
#define MCUIO_GPIO_INPUT  1

typedef enum
{
    MCUIO_GPIO_ID_RESERVED = 0
} mcuio_gpio_id_t;

typedef enum
{
    MCUIO_PWM_ID_RESERVED = 0
} mcuio_pwm_id_t;

typedef enum
{
    MCUIO_UART_ID_RESERVED = 0
} mcuio_uart_id_t;

typedef enum
{
    MCUIO_ENCODER_ID_RESERVED = 0
} mcuio_encoder_id_t;

typedef enum
{
    MCUIO_PIT_ID_RESERVED = 0
} mcuio_pit_id_t;

void McuIo_GpioInit(mcuio_gpio_id_t pin, uint8_t mode);
void McuIo_GpioHigh(mcuio_gpio_id_t pin);
void McuIo_GpioLow(mcuio_gpio_id_t pin);

void McuIo_PwmInit(mcuio_pwm_id_t ch, uint32_t freq_hz, uint32_t duty);
void McuIo_PwmSetDuty(mcuio_pwm_id_t ch, uint32_t duty);

void McuIo_UartInit(mcuio_uart_id_t ch, uint32_t baud);

void McuIo_EncoderInit(mcuio_encoder_id_t ch);
int32_t McuIo_EncoderGet(mcuio_encoder_id_t ch);
void McuIo_EncoderClear(mcuio_encoder_id_t ch);

void McuIo_PitInit(mcuio_pit_id_t ch, uint32_t period_ms);
void McuIo_PitClearFlag(mcuio_pit_id_t ch);

//******************************** Device Port *************************************//
typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} camera_frame_desc_t;

void     Device_CameraInit(void);
bool     Device_CameraReady(void);
void     Device_CameraClear(void);
uint8_t *Device_CameraData(void);
void     Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc);

void Device_DisplayInit(void);
void Device_DisplayPoint(int16_t x, int16_t y, uint16_t color);
void Device_DisplayGray(int16_t x, int16_t y, const uint8_t *p_img,
                        uint16_t w, uint16_t h,
                        uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void Device_DisplayStr(int16_t x, int16_t y, const char *p_str);
void Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits);

void  Device_ImuInit(void);
void  Device_ImuRead(void);
float Device_ImuZ(void);

void Device_WirelessInit(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_PORT_IF_H_ */
