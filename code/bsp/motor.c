/*
 * motor.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 *
 * @brief 电机驱动 + H 桥双通道 PWM 控制
 *        面向对象封装：全部运行时状态收敛到 motor_t 结构体
 *        内部函数以 motor_t* 为第一参数，公开 API 包装单实例
 */

#include "motor.h"
#include "config.h"

/* ===== 电机对象（封装全部运行时状态）===== */
/* 单实例（全项目仅一组电机） */
static motor_t s_motor = { 0 };

/* ===== 内部操作（均以 motor_t* 为第一参数）===== */

/**
 * @brief 速度限幅到 [-clamp, +clamp]
 */
static int32_t motor_clamp_speed(int32_t speed, int32_t clamp)
{
    if (speed > clamp)
    {
        return clamp;
    }
    if (speed < -clamp)
    {
        return -clamp;
    }
    return speed;
}

/**
 * @brief 写入左轮 H 桥 PWM（正值正转，负值反转）
 *        占空比缩放因子 100，与原实现保持一致
 */
static void motor_apply_left(motor_t *m, int32_t speed)
{
    m->speed_l = speed;
    if (speed > 0)
    {
        /* 正转：正转通道出 PWM，反转通道清零 */
        pal_pwm_set_duty(PAL_CH_MOTOR_L_FWD, (uint32_t)(speed * 100));
        pal_pwm_set_duty(PAL_CH_MOTOR_L_REV, 0);
    }
    else
    {
        /* 反转：反转通道出 PWM，正转通道清零（speed 取负转为正占空比） */
        pal_pwm_set_duty(PAL_CH_MOTOR_L_REV, (uint32_t)(-speed * 100));
        pal_pwm_set_duty(PAL_CH_MOTOR_L_FWD, 0);
    }
}

/**
 * @brief 写入右轮 H 桥 PWM
 */
static void motor_apply_right(motor_t *m, int32_t speed)
{
    m->speed_r = speed;
    if (speed > 0)
    {
        pal_pwm_set_duty(PAL_CH_MOTOR_R_FWD, (uint32_t)(speed * 100));
        pal_pwm_set_duty(PAL_CH_MOTOR_R_REV, 0);
    }
    else
    {
        pal_pwm_set_duty(PAL_CH_MOTOR_R_REV, (uint32_t)(-speed * 100));
        pal_pwm_set_duty(PAL_CH_MOTOR_R_FWD, 0);
    }
}

/* ===== 公开 API（包装单实例 s_motor）===== */

void Motor_Init(void)
{
    s_motor.clamp_l = MOTOR_CLAMP_LEFT;
    s_motor.clamp_r = MOTOR_CLAMP_RIGHT;
    s_motor.speed_l = 0;
    s_motor.speed_r = 0;

    /* 4 路 PWM 初始化：频率 MOTOR_PWM_HZ，初始占空比 0（电机停转） */
    pal_pwm_init(PAL_CH_MOTOR_R_REV, MOTOR_PWM_HZ, 0);
    pal_pwm_init(PAL_CH_MOTOR_R_FWD, MOTOR_PWM_HZ, 0);
    pal_pwm_init(PAL_CH_MOTOR_L_REV, MOTOR_PWM_HZ, 0);
    pal_pwm_init(PAL_CH_MOTOR_L_FWD, MOTOR_PWM_HZ, 0);
}

void Motor_SetLeft(int32_t speed)
{
    motor_apply_left(&s_motor, motor_clamp_speed(speed, s_motor.clamp_l));
}

void Motor_SetRight(int32_t speed)
{
    motor_apply_right(&s_motor, motor_clamp_speed(speed, s_motor.clamp_r));
}
