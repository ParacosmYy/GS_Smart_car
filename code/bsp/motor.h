/*
 * motor.h
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 *
 * @brief 电机驱动（H 桥双通道 PWM）
 *        面向对象封装：左右轮速度与限幅收敛到 motor_t 结构体
 *        内部函数以 motor_t* 为第一参数，公开 API 包装单实例
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "pal_pwm.h"

/**
 * @brief 电机驱动对象（封装左右轮当前速度与限幅值）
 *        字段对上层只读：修改请走 Motor_* API
 */
typedef struct {
    int32_t speed_l;    /* 左电机当前速度（带符号） */
    int32_t speed_r;    /* 右电机当前速度（带符号） */
    int32_t clamp_l;    /* 左电机速度限幅（绝对值） */
    int32_t clamp_r;    /* 右电机速度限幅（绝对值） */
} motor_t;

/**
 * @brief 电机初始化
 *        配置 4 路 PWM 通道（左右各正反两路），初始占空比 0
 *        限幅从 config.h 装载（MOTOR_CLAMP_LEFT / _RIGHT）
 */
void Motor_Init(void);

/**
 * @brief 设置左轮速度（单实例包装）
 * @param speed 目标速度，带符号；正值正转，负值反转
 */
void Motor_SetLeft(int32_t speed);

/**
 * @brief 设置右轮速度（单实例包装）
 * @param speed 目标速度，带符号；正值正转，负值反转
 */
void Motor_SetRight(int32_t speed);

#endif /* CODE_MOTOR_H_ */
