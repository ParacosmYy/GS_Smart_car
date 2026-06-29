#ifndef SMARTCAR_BOARD_RESOURCES_H_
#define SMARTCAR_BOARD_RESOURCES_H_

/**
 * @file smartcar_board_resources.h
 * @brief 本车板级资源编号（Product Board Config）。
 *
 * 这些枚举定义当前智能车产品的具体外设通道分配。
 * 换板子（不同硬件布局）时应在此文件调整映射关系。
 *
 * 本文件属于产品配置，Platform 接口不引用此文件。
 */

/* ── PWM 通道 ────────────────────────────────────────── */
typedef enum
{
    SMARTCAR_PWM_MOTOR_R_FWD = 0,
    SMARTCAR_PWM_MOTOR_R_REV,
    SMARTCAR_PWM_MOTOR_L_FWD,
    SMARTCAR_PWM_MOTOR_L_REV,
    SMARTCAR_PWM_SERVO,
    SMARTCAR_PWM_ID_MAX
} smartcar_pwm_id_t;

/* ── GPIO 引脚 ───────────────────────────────────────── */
typedef enum
{
    SMARTCAR_GPIO_BUZZER = 0,
    SMARTCAR_GPIO_KEY1,
    SMARTCAR_GPIO_KEY2,
    SMARTCAR_GPIO_KEY3,
    SMARTCAR_GPIO_KEY4,
    SMARTCAR_GPIO_DIP1,
    SMARTCAR_GPIO_DIP2,
    SMARTCAR_GPIO_DIP3,
    SMARTCAR_GPIO_DIP4,
    SMARTCAR_GPIO_ID_MAX
} smartcar_gpio_id_t;

/* ── 编码器通道 ──────────────────────────────────────── */
typedef enum
{
    SMARTCAR_ENCODER_LEFT = 0,
    SMARTCAR_ENCODER_RIGHT,
    SMARTCAR_ENCODER_ID_MAX
} smartcar_encoder_id_t;

/* ── PIT 周期中断通道 ────────────────────────────────── */
typedef enum
{
    SMARTCAR_PIT_ENCODER_SAMPLE = 0,
    SMARTCAR_PIT_GYRO_TICK,
    SMARTCAR_PIT_SPARE_2,
    SMARTCAR_PIT_SPARE_3,
    SMARTCAR_PIT_ID_MAX
} smartcar_pit_id_t;

/* ── UART 串口通道 ───────────────────────────────────── */
typedef enum
{
    SMARTCAR_UART_CAMERA = 0,
    SMARTCAR_UART_BT,
    SMARTCAR_UART_ID_MAX
} smartcar_uart_id_t;

#endif /* SMARTCAR_BOARD_RESOURCES_H_ */
