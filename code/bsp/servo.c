/*
 * servo.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 */

#include "servo.h"
#include "config.h"

// 舵机 PWM 占空比的三个关键值：中位、右打最大、左打最大
#define servo_mid SERVO_CENTER_DUTY
#define servo_max (SERVO_CENTER_DUTY + SERVO_RANGE)
#define servo_min (SERVO_CENTER_DUTY - SERVO_RANGE)

// 舵机当前实际占空比（外部可见）
uint32_t servo_duty = 0;

/**
 * @brief 舵机初始化
 *         在 PAL_CH_SERVO 上输出 SERVO_PWM_HZ 的 PWM
 *         初始占空比为 SERVO_INIT_DUTY，舵机居中
 */
void servo_init(void)
{
    pal_pwm_init(PAL_CH_SERVO, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}


