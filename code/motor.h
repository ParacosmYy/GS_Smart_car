/*
 * motor.h
 *
 *  Created on: 2025Äê10ÔÂ19ÈÕ
 *      Author: Paracosm
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

extern uint32 motor_speed;


void motor_init(void);
void motor_setspeed_left(int speed);//0--100
void motor_setspeed_right(int speed);//0--100

#endif /* CODE_MOTOR_H_ */
