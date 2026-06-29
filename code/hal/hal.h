#ifndef CODE_HAL_HAL_H_
#define CODE_HAL_HAL_H_

/**
 * @file hal.h
 * @brief Smart car HAL contract shared by upper layers and target ports.
 *
 * Target ports provide the SmartcarHal_* symbols at link time. This public
 * contract intentionally avoids vendor, target, and board-resource headers.
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SMARTCAR_HAL_GPIO_OUTPUT 0U
#define SMARTCAR_HAL_GPIO_INPUT  1U

typedef enum
{
    SMARTCAR_HAL_GPIO_ID_RESERVED = 0
} smartcar_hal_gpio_id_t;

typedef enum
{
    SMARTCAR_HAL_PWM_ID_RESERVED = 0
} smartcar_hal_pwm_id_t;

typedef enum
{
    SMARTCAR_HAL_UART_ID_RESERVED = 0
} smartcar_hal_uart_id_t;

typedef enum
{
    SMARTCAR_HAL_ENCODER_ID_RESERVED = 0
} smartcar_hal_encoder_id_t;

typedef enum
{
    SMARTCAR_HAL_PIT_ID_RESERVED = 0
} smartcar_hal_pit_id_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
} smartcar_hal_camera_desc_t;

void SmartcarHal_ClockInit(void);
void SmartcarHal_DebugInit(void);
void SmartcarHal_CoreSync(void);
uint32_t SmartcarHal_IrqDisable(void);
void SmartcarHal_IrqRestore(uint32_t state);

void SmartcarHal_GpioInit(smartcar_hal_gpio_id_t pin, uint8_t mode);
void SmartcarHal_GpioHigh(smartcar_hal_gpio_id_t pin);
void SmartcarHal_GpioLow(smartcar_hal_gpio_id_t pin);

void SmartcarHal_PwmInit(smartcar_hal_pwm_id_t channel, uint32_t freq_hz, uint32_t duty);
void SmartcarHal_PwmSetDuty(smartcar_hal_pwm_id_t channel, uint32_t duty);

void SmartcarHal_UartInit(smartcar_hal_uart_id_t channel, uint32_t baud);

void SmartcarHal_EncoderInit(smartcar_hal_encoder_id_t channel);
int32_t SmartcarHal_EncoderGet(smartcar_hal_encoder_id_t channel);
void SmartcarHal_EncoderClear(smartcar_hal_encoder_id_t channel);
void SmartcarHal_EncoderTakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

void SmartcarHal_PitInit(smartcar_hal_pit_id_t channel, uint32_t period_ms);
void SmartcarHal_PitClearFlag(smartcar_hal_pit_id_t channel);

void     SmartcarHal_CameraInit(void);
bool     SmartcarHal_CameraReady(void);
void     SmartcarHal_CameraClear(void);
uint8_t *SmartcarHal_CameraData(void);
void     SmartcarHal_CameraGetDesc(smartcar_hal_camera_desc_t *p_desc);

void SmartcarHal_DisplayInit(void);
void SmartcarHal_DisplayPoint(int16_t x, int16_t y, uint16_t color);
void SmartcarHal_DisplayGray(int16_t x, int16_t y, const uint8_t *p_img,
                             uint16_t w, uint16_t h,
                             uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void SmartcarHal_DisplayStr(int16_t x, int16_t y, const char *p_str);
void SmartcarHal_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits);

void  SmartcarHal_ImuInit(void);
void  SmartcarHal_ImuRead(void);
float SmartcarHal_ImuZ(void);

void SmartcarHal_WirelessInit(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_HAL_HAL_H_ */
