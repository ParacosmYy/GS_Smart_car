/*
 * motor.c
 *
 *  Created on: 2025Äê10ÔÂ19ÈÕ
 *      Author: Paracosm
 */
#include "motor.h"


uint32 motor_speed = 0;



void motor_init(void)
{
   gpio_init(P21_2,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_3,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_4,GPO,0,GPO_PUSH_PULL);
   gpio_init(P21_5,GPO,0,GPO_PUSH_PULL);
   pwm_init(ATOM0_CH0_P21_2,20000,0);
   pwm_init(ATOM0_CH1_P21_3,20000,0);
   pwm_init(ATOM0_CH2_P21_4,20000,0);
   pwm_init(ATOM0_CH3_P21_5,20000,0);
}

void motor_setspeed_right(int speed)//-100--100
{
    if(speed > 100)
    {
        speed = 100;
    }
    else if(speed < -100)
    {
        speed = -100;
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
    if(speed > 15)
    {
       speed = 15;
    }
    else if(speed < -15)
    {
       speed = -15;
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

