/**
 * @file target_board_map.c
 * @brief TC264 板级引脚/通道物理映射表。
 * @author GS_Mark
 *
 * @par 设计说明
 * 产品资源 ID 在 code/config 中定义，本文件只负责把资源 ID 映射为 TC264 Vendor 常量。
 */

#include "target_board_map.h"

#include "smartcar_board_resources.h"
#include "zf_common_headfile.h"

/** @brief 产品 PWM 资源到 TC264 ATOM 通道的映射表。 */
const uint32_t g_tc264_pwm_map[SMARTCAR_PWM_ID_MAX] =
{
    [SMARTCAR_PWM_MOTOR_R_FWD] = ATOM0_CH1_P21_3,
    [SMARTCAR_PWM_MOTOR_R_REV] = ATOM0_CH0_P21_2,
    [SMARTCAR_PWM_MOTOR_L_FWD] = ATOM0_CH3_P21_5,
    [SMARTCAR_PWM_MOTOR_L_REV] = ATOM0_CH2_P21_4,
    [SMARTCAR_PWM_SERVO]       = ATOM1_CH1_P33_9,
};

/** @brief 产品 GPIO 资源到 TC264 端口引脚的映射表。 */
const uint32_t g_tc264_gpio_map[SMARTCAR_GPIO_ID_MAX] =
{
    [SMARTCAR_GPIO_BUZZER] = P11_11,
    [SMARTCAR_GPIO_KEY1]   = P20_6,
    [SMARTCAR_GPIO_KEY2]   = P20_7,
    [SMARTCAR_GPIO_KEY3]   = P20_8,
    [SMARTCAR_GPIO_KEY4]   = P20_9,
    [SMARTCAR_GPIO_DIP1]   = P15_5,
    [SMARTCAR_GPIO_DIP2]   = P15_6,
    [SMARTCAR_GPIO_DIP3]   = P15_8,
    [SMARTCAR_GPIO_DIP4]   = P15_9,
};

/** @brief 产品编码器资源到 TC264 TIM 编码器通道的映射表。 */
const tc264_encoder_map_t g_tc264_encoder_map[SMARTCAR_ENCODER_ID_MAX] =
{
    [SMARTCAR_ENCODER_LEFT]  = { TIM2_ENCODER, TIM2_ENCODER_CH1_P33_7, TIM2_ENCODER_CH2_P33_6 },
    [SMARTCAR_ENCODER_RIGHT] = { TIM4_ENCODER, TIM4_ENCODER_CH1_P02_8, TIM4_ENCODER_CH2_P00_9 },
};

/** @brief 产品 PIT 资源到 TC264 CCU6 通道的映射表。 */
const uint32_t g_tc264_pit_map[SMARTCAR_PIT_ID_MAX] =
{
    [SMARTCAR_PIT_ENCODER_SAMPLE] = CCU60_CH0,
    [SMARTCAR_PIT_GYRO_TICK]      = CCU60_CH1,
    [SMARTCAR_PIT_SPARE_2]        = CCU61_CH0,
    [SMARTCAR_PIT_SPARE_3]        = CCU61_CH1,
};

/** @brief 产品 UART 资源到 TC264 UART 引脚组合的映射表。 */
const tc264_uart_map_t g_tc264_uart_map[SMARTCAR_UART_ID_MAX] =
{
    [SMARTCAR_UART_CAMERA] = { UART_1, UART1_TX_P02_2, UART1_RX_P02_3 },
    [SMARTCAR_UART_BT]     = { UART_3, UART3_TX_P15_7, UART3_RX_P20_3 },
};
