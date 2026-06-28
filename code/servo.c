/*
 * servo.c
 *
 *  Created on: 2025Äê10ÔÂ21ÈÕ
 *      Author: Paracosm
 */

#include "servo.h"
#define servo_mid 678
#define servo_max (678+63)
#define servo_min (678-63)

uint32 servo_duty = 0;

void servo_init(void)
{
    pwm_init (ATOM1_CH1_P33_9 , 50, 692);
}


