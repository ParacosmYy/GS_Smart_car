/*
 * motor.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */
#include "motor.h"
#include "config.h"


// 当前电机目标速度（占位用，外部可读取）
uint32 motor_speed = 0;



/**
 * @brief 电机初始化
 *         配置 4 路 GPIO 为推挽输出，再初始化 4 路 ATOM PWM 通道
 *         P21_2/P21_3 控制右轮正反转，P21_4/P21_5 控制左轮正反转
 */
void motor_init(void)
{
   // 先将 4 个引脚配置为 GPO 推挽输出，初始电平 0
   gpio_init(P21_2,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_3,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_4,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_5,GPO,0,GPO_PUSH_PULL);
   // 4 路 PWM 初始化，频率 MOTOR_PWM_HZ，初始占空比为 0（电机停转）
   pwm_init(ATOM0_CH0_P21_2,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH1_P21_3,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH2_P21_4,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH3_P21_5,MOTOR_PWM_HZ,0);
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
        // 正转：CH1 输出 PWM，CH0 清零
        pwm_set_duty(ATOM0_CH1_P21_3,speed * 100);
        pwm_set_duty(ATOM0_CH0_P21_2, 0);
    }
    else
    {
        // 反转：CH0 输出 PWM，CH1 清零（speed 取负转为正占空比）
        pwm_set_duty(ATOM0_CH0_P21_2, -speed * 100);
        pwm_set_duty(ATOM0_CH1_P21_3, 0);
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
        // 正转：CH3 输出 PWM，CH2 清零
        pwm_set_duty(ATOM0_CH3_P21_5,speed * 100);
        pwm_set_duty(ATOM0_CH2_P21_4, 0);
    }
    else
    {
        // 反转：CH2 输出 PWM，CH3 清零
        pwm_set_duty(ATOM0_CH2_P21_4, -speed * 100);
        pwm_set_duty(ATOM0_CH3_P21_5, 0);
    }
}

