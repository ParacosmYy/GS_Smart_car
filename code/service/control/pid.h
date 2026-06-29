/*
 * @file pid.h
 *
 * @brief PID algorithm interface.
 *
 * This header declares positional/incremental PID types, init/calc APIs, and
 * bounded steering/motor helper controllers.
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */

#ifndef CODE_SERVICE_CONTROL_PID_H_
#define CODE_SERVICE_CONTROL_PID_H_

/**
 * @brief 位置式PID结构体
 *        输出 = Kp*err + Ki*Σerr + Kd*(err-prev_err)
 */
typedef struct {
    float kp, ki, kd;   ///< 比例/积分/微分系数 Kp Ki Kd
    float integral;     ///< 积分累加项
    float prev_err;     ///< 上次偏差（用于微分）
    float output;       ///< PID输出
} PosPID_t;

/**
 * @brief 增量式PID结构体
 *        输出 = 上次输出 + Δu，Δu由本次/上次/上上次偏差推得
 */
typedef struct {
    float kp, ki, kd;       ///< 比例/积分/微分系数 Kp Ki Kd
    float prev_err;         ///< 上次偏差
    float prev_prev_err;    ///< 上上次偏差
    float output;           ///< 已限幅的累计输出
} IncPID_t;

void PosPID_Init(PosPID_t *pid, float kp, float ki, float kd);            ///< 位置式PID初始化
void IncPID_Init(IncPID_t *pid, float kp, float ki, float kd);            ///< 增量式PID初始化
float PosPID_Calc(PosPID_t *pid, float target, float feedback);          ///< 位置式PID计算
float IncPID_Calc(IncPID_t *pid, float target, float feedback);          ///< 增量式PID计算（累计输出限幅）
float ServoPid_Control(PosPID_t *steer_pid, float steer_target, float steer_feedback);      ///< 舵机PID控制（位置式，限幅±63）
float MotorPid_Control(IncPID_t *motor_pid, float speed_target, float speed_feedback);      ///< 电机PID控制（增量式，内部累计与返回值限幅±20）

#endif /* CODE_SERVICE_CONTROL_PID_H_ */
