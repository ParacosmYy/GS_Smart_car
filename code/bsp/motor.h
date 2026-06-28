/*
 * motor.h
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "platform.h"

// 电机当前目标速度（外部可见）
extern uint32_t motor_speed;


/**
 * @brief 电机初始化，初始化 4 路 PWM 通道（PAL 层内部配置引脚）
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
