/*
 * servo.h
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 *
 * @brief 舵机驱动（50Hz PWM）
 *        面向对象封装：占空比/中位/范围收敛到 servo_t 结构体
 *        内部函数以 servo_t* 为第一参数，公开 API 包装单实例
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#include "platform.h"

/**
 * @brief 舵机驱动对象（封装当前占空比、机械中位、最大偏转范围）
 *        字段对上层只读：修改请走 Servo_* API
 */
typedef struct {
    uint32_t duty;      /* 当前 PWM 占空比 */
    uint32_t center;    /* 机械中位占空比（装车标定） */
    uint32_t range;     /* 最大偏转范围（占空比差值） */
} servo_t;

/**
 * @brief 舵机初始化
 *        启动 PAL_CH_SERVO 上 SERVO_PWM_HZ 的 PWM，初始居中
 *        中位与范围从 config.h 装载（SERVO_CENTER_DUTY / SERVO_RANGE）
 */
void Servo_Init(void);

/**
 * @brief 设置舵机偏转角度（单实例包装）
 *        内部完成限幅到 [center - range, center + range]，防止打死方向
 * @param offset 相对中位的偏移量（占空比差值），来自 PID 输出
 *               正值往一边打，负值往另一边打
 */
void Servo_SetAngle(int32_t offset);

#endif /* CODE_SERVO_H_ */
