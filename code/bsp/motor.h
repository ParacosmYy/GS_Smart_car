/*
 * motor.h
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

// 电机当前目标速度（外部可见）
extern uint32 motor_speed;


/**
 * @brief 电机初始化，配置 4 路 GPIO 与 ATOM PWM 通道
 */
void motor_init(void);

/**
 * @brief 设置左轮速度
 * @param speed 目标速度，范围 -100~+100；正值正转，负值反转
 */
void motor_setspeed_left(int speed);

/**
 * @brief 设置右轮速度
 * @param speed 目标速度，范围 -100~+100；正值正转，负值反转
 */
void motor_setspeed_right(int speed);

#endif /* CODE_MOTOR_H_ */
