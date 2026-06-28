/*
 * motor.c
 *
 *  Created on: 2025Äê10ÔÂ19ÈÕ
 *      Author: Paracosm
 */
#include "motor.h"
#include "config.h"


uint32 motor_speed = 0;



void motor_init(void)
{
   gpio_init(P21_2,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_3,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_4,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_5,GPO,0,GPO_PUSH_PULL);
   pwm_init(ATOM0_CH0_P21_2,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH1_P21_3,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH2_P21_4,MOTOR_PWM_HZ,0);
   pwm_init(ATOM0_CH3_P21_5,MOTOR_PWM_HZ,0);
}

void motor_setspeed_right(int speed)//-100--100
{
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
        pwm_set_duty(ATOM0_CH1_P21_3,speed * 100);
        pwm_set_duty(ATOM0_CH0_P21_2, 0);
    }
    else
    {
        pwm_set_duty(ATOM0_CH0_P21_2, -speed * 100);
        pwm_set_duty(ATOM0_CH1_P21_3, 0);
    }
}

void motor_setspeed_left(int speed)
{
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
        pwm_set_duty(ATOM0_CH3_P21_5,speed * 100);
        pwm_set_duty(ATOM0_CH2_P21_4, 0);
    }
    else
    {
        pwm_set_duty(ATOM0_CH2_P21_4, -speed * 100);
        pwm_set_duty(ATOM0_CH3_P21_5, 0);
    }
}

