#ifndef SMARTCAR_BOARD_RESOURCES_H_
#define SMARTCAR_BOARD_RESOURCES_H_

/**
 * @file smartcar_board_resources.h
 * @brief 本车板级资源编号（Product Board Config）。
 *
 * 这些资源编号定义当前智能车产品的具体外设通道分配。
 * 换板子（不同硬件布局）时应在此文件调整映射关系。
 *
 * 本文件属于产品配置，Platform 接口不引用此文件。
 */

#include "platform/port_if.h"

/* ── PWM 通道 ────────────────────────────────────────── */
typedef mcuio_pwm_id_t smartcar_pwm_id_t;

#define SMARTCAR_PWM_MOTOR_R_FWD ((smartcar_pwm_id_t)0)
#define SMARTCAR_PWM_MOTOR_R_REV ((smartcar_pwm_id_t)1)
#define SMARTCAR_PWM_MOTOR_L_FWD ((smartcar_pwm_id_t)2)
#define SMARTCAR_PWM_MOTOR_L_REV ((smartcar_pwm_id_t)3)
#define SMARTCAR_PWM_SERVO       ((smartcar_pwm_id_t)4)
#define SMARTCAR_PWM_ID_MAX      5

/* ── GPIO 引脚 ───────────────────────────────────────── */
typedef mcuio_gpio_id_t smartcar_gpio_id_t;

#define SMARTCAR_GPIO_BUZZER ((smartcar_gpio_id_t)0)
#define SMARTCAR_GPIO_KEY1   ((smartcar_gpio_id_t)1)
#define SMARTCAR_GPIO_KEY2   ((smartcar_gpio_id_t)2)
#define SMARTCAR_GPIO_KEY3   ((smartcar_gpio_id_t)3)
#define SMARTCAR_GPIO_KEY4   ((smartcar_gpio_id_t)4)
#define SMARTCAR_GPIO_DIP1   ((smartcar_gpio_id_t)5)
#define SMARTCAR_GPIO_DIP2   ((smartcar_gpio_id_t)6)
#define SMARTCAR_GPIO_DIP3   ((smartcar_gpio_id_t)7)
#define SMARTCAR_GPIO_DIP4   ((smartcar_gpio_id_t)8)
#define SMARTCAR_GPIO_ID_MAX 9

/* ── 编码器通道 ──────────────────────────────────────── */
typedef mcuio_encoder_id_t smartcar_encoder_id_t;

#define SMARTCAR_ENCODER_LEFT   ((smartcar_encoder_id_t)0)
#define SMARTCAR_ENCODER_RIGHT  ((smartcar_encoder_id_t)1)
#define SMARTCAR_ENCODER_ID_MAX 2

/* ── PIT 周期中断通道 ────────────────────────────────── */
typedef mcuio_pit_id_t smartcar_pit_id_t;

#define SMARTCAR_PIT_ENCODER_SAMPLE ((smartcar_pit_id_t)0)
#define SMARTCAR_PIT_GYRO_TICK      ((smartcar_pit_id_t)1)
#define SMARTCAR_PIT_SPARE_2        ((smartcar_pit_id_t)2)
#define SMARTCAR_PIT_SPARE_3        ((smartcar_pit_id_t)3)
#define SMARTCAR_PIT_ID_MAX         4

/* ── UART 串口通道 ───────────────────────────────────── */
typedef mcuio_uart_id_t smartcar_uart_id_t;

#define SMARTCAR_UART_CAMERA ((smartcar_uart_id_t)0)
#define SMARTCAR_UART_BT     ((smartcar_uart_id_t)1)
#define SMARTCAR_UART_ID_MAX 2

#endif /* SMARTCAR_BOARD_RESOURCES_H_ */
