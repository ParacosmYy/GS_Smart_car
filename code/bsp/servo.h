/*
 * servo.h
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#include "zf_common_headfile.h"

// 舵机当前 PWM 占空比（外部可见）
extern uint32 servo_duty ;

/**
 * @brief 舵机初始化，配置 ATOM1_CH1（P33_9）输出 PWM 并居中
 */
void servo_init(void);
#endif /* CODE_SERVO_H_ */
