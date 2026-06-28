/*
 * motor.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */
#include "motor.h"
#include "config.h"


// 当前电机目标速度（占位用，外部可读取）
uint32_t motor_speed = 0;



/**
 * @brief 电机初始化
 *         初始化 4 路 PWM 通道（PAL 层内部已配置引脚）
 *         右轮：PAL_CH_MOTOR_R_FWD / PAL_CH_MOTOR_R_REV
 *         左轮：PAL_CH_MOTOR_L_FWD / PAL_CH_MOTOR_L_REV
 */
void motor_init(void)
{
   // 4 路 PWM 初始化，频率 MOTOR_PWM_HZ，初始占空比为 0（电机停转）
   pal_pwm_init(PAL_CH_MOTOR_R_REV, MOTOR_PWM_HZ, 0);
   pal_pwm_init(PAL_CH_MOTOR_R_FWD, MOTOR_PWM_HZ, 0);
   pal_pwm_init(PAL_CH_MOTOR_L_REV, MOTOR_PWM_HZ, 0);
   pal_pwm_init(PAL_CH_MOTOR_L_FWD, MOTOR_PWM_HZ, 0);
}

/**
 * @brief 设置右轮速度（H 桥双通道控制方向）
 * @param speed 目标速度，范围 -100~+100；正值正转，负值反转
 */
void motor_setspeed_right(int speed)
{
    // 限幅：防止速度超出电机物理极限
    if(speed > MOTOR_CLAMP_RIGHT)
    {
        speed = MOTOR_CLAMP_RIGHT;
    }
    else if(speed < -MOTOR_CLAMP_RIGHT)
    {
        speed = -MOTOR_CLAMP_RIGHT;
    }
    if(speed > 0)
    {
        // 正转：右电机正转通道输出 PWM，反转通道清零
        pal_pwm_set_duty(PAL_CH_MOTOR_R_FWD, speed * 100);
        pal_pwm_set_duty(PAL_CH_MOTOR_R_REV, 0);
    }
    else
    {
        // 反转：右电机反转通道输出 PWM，正转通道清零（speed 取负转为正占空比）
        pal_pwm_set_duty(PAL_CH_MOTOR_R_REV, -speed * 100);
        pal_pwm_set_duty(PAL_CH_MOTOR_R_FWD, 0);
    }
}

/**
 * @brief 设置左轮速度（H 桥双通道控制方向）
 * @param speed 目标速度，范围 -100~+100；正值正转，负值反转
 */
void motor_setspeed_left(int speed)
{
    // 限幅：防止速度超出电机物理极限
    if(speed > MOTOR_CLAMP_LEFT)
    {
       speed = MOTOR_CLAMP_LEFT;
    }
    else if(speed < -MOTOR_CLAMP_LEFT)
    {
       speed = -MOTOR_CLAMP_LEFT;
    }
    if(speed > 0)
    {
        // 正转：左电机正转通道输出 PWM，反转通道清零
        pal_pwm_set_duty(PAL_CH_MOTOR_L_FWD, speed * 100);
        pal_pwm_set_duty(PAL_CH_MOTOR_L_REV, 0);
    }
    else
    {
        // 反转：左电机反转通道输出 PWM，正转通道清零
        pal_pwm_set_duty(PAL_CH_MOTOR_L_REV, -speed * 100);
        pal_pwm_set_duty(PAL_CH_MOTOR_L_FWD, 0);
    }
}

