#ifndef PAL_RESOURCES_H_
#define PAL_RESOURCES_H_

/**
 * @file pal_resources.h
 * @brief PAL 逻辑资源编号。
 */

typedef enum
{
    PAL_CH_MOTOR_R_FWD = 0,
    PAL_CH_MOTOR_R_REV,
    PAL_CH_MOTOR_L_FWD,
    PAL_CH_MOTOR_L_REV,
    PAL_CH_SERVO,
    PAL_PWM_ID_MAX
} pal_pwm_id_t;

typedef enum
{
    PAL_PIN_BUZZER = 0,
    PAL_PIN_KEY1,
    PAL_PIN_KEY2,
    PAL_PIN_KEY3,
    PAL_PIN_KEY4,
    PAL_PIN_DIP1,
    PAL_PIN_DIP2,
    PAL_PIN_DIP3,
    PAL_PIN_DIP4,
    PAL_GPIO_ID_MAX
} pal_gpio_id_t;

typedef enum
{
    PAL_CH_ENCODER_L = 0,
    PAL_CH_ENCODER_R,
    PAL_ENCODER_ID_MAX
} pal_encoder_id_t;

typedef enum
{
    PAL_CH_PIT_0 = 0,
    PAL_CH_PIT_1,
    PAL_CH_PIT_2,
    PAL_CH_PIT_3,
    PAL_PIT_ID_MAX
} pal_pit_id_t;

typedef enum
{
    PAL_CH_UART_CAM = 0,
    PAL_CH_UART_BT,
    PAL_UART_ID_MAX
} pal_uart_id_t;

#endif /* PAL_RESOURCES_H_ */
