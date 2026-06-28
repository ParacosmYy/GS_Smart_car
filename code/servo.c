/*
 * servo.c
 *
 *  Created on: 2025Äê10ÔÂ21ÈÕ
 *      Author: Paracosm
 */

#include "servo.h"
#include "config.h"
#define servo_mid SERVO_CENTER_DUTY
#define servo_max (SERVO_CENTER_DUTY + SERVO_RANGE)
#define servo_min (SERVO_CENTER_DUTY - SERVO_RANGE)

uint32 servo_duty = 0;

void servo_init(void)
{
    pwm_init(ATOM1_CH1_P33_9, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}


