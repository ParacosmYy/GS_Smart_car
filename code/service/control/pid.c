/*
 * pid.c
 *
 * PID 算法实现：位置式 + 增量式 PID，以及舵机/电机专用控制函数。
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */
#include "pid.h"
#include "zf_common_headfile.h"

// PID输出结果（供 Actuator_Apply 使用）
float servo_pid_output = 0.0;
float left_motor_pid_output = 0;
float right_motor_pid_output = 0;

// PID结构体实例
PosPID_t servo_pid  ;
IncPID_t left_motor_pid ;
IncPID_t right_motor_pid ;

/**
 * @brief 位置式PID参数初始化
 * @param pid   PID结构体指针
 * @param kp    比例系数 Kp
 * @param ki    积分系数 Ki
 * @param kd    微分系数 Kd
 */
void PosPID_Init(PosPID_t *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0;   // 积分项清零
    pid->prev_err = 0;   // 上次偏差清零
    pid->output = 0;     // 输出清零
}

/**
 * @brief 增量式PID参数初始化
 * @param pid   PID结构体指针
 * @param kp    比例系数 Kp
 * @param ki    积分系数 Ki
 * @param kd    微分系数 Kd
 */
void IncPID_Init(IncPID_t *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_err = 0;       // 上次偏差清零
    pid->prev_prev_err = 0;  // 上上次偏差清零
    pid->output = 0;         // 累计输出清零
}

/**
 * @brief 位置式PID计算
 *        输出 = Kp*err + Ki*积分 + Kd*微分，每次输出为绝对值
 * @param pid       PID结构体指针
 * @param target    目标值
 * @param feedback  反馈值（实测）
 * @return          PID输出
 */
float PosPID_Calc(PosPID_t *pid, float target, float feedback) {
    // 当前偏差
    float err = target - feedback;
    // 积分项：累加历史偏差
    pid->integral += err;
    // 微分项：当前偏差与上次偏差之差
    float derivative = err - pid->prev_err;
    // PID输出 = 比例项 + 积分项 + 微分项
    pid->output = pid->kp * err + pid->ki * pid->integral + pid->kd * derivative;
    // 保存本次偏差供下次微分使用
    pid->prev_err = err;
    return pid->output;
}

/**
 * @brief 增量式PID计算
 *        输出 = 上次输出 + 本次增量 Δu
 *        Δu = Kp*(err-prev_err) + Ki*err + Kd*(err-2*prev_err+prev_prev_err)
 * @param pid       PID结构体指针
 * @param target    目标值
 * @param feedback  反馈值（实测）
 * @return          PID累计输出
 */
float IncPID_Calc(IncPID_t *pid, float target, float feedback) {
    // 当前偏差
    float err = target - feedback;
    // 计算增量 Δu = Kp*(err-prev_err) + Ki*err + Kd*(err-2*prev_err+prev_prev_err)
    float delta_output = pid->kp * (err - pid->prev_err)
                      + pid->ki * err
                      + pid->kd * (err - 2 * pid->prev_err + pid->prev_prev_err);
    // 累加到上次输出得到本次输出
    pid->output += delta_output;
    // 偏差历史滚动：prev_err -> prev_prev_err，err -> prev_err
    pid->prev_prev_err = pid->prev_err;
    pid->prev_err = err;
    return pid->output;
}

/**
 * @brief 舵机PID控制
 *        位置式PID，根据中线偏差计算舵机转角输出
 * @param pid            PID结构体指针
 * @param steer_target   目标值（0=居中）
 * @param steer_feedback 反馈值（视觉中线偏差 calculate_error）
 * @return 舵机PID输出（限幅 ±63）
 *
 * 限幅说明：±63 对应 SERVO_RANGE，舵机PWM增量的最大允许范围，
 * 超过此值会导致舵机打到机械极限并可能损坏。
 */
float servo_pid_contorl(PosPID_t *steer_pid , float steer_target , float steer_feedback)
{
    // 位置式PID计算原始输出
    float servo_output = PosPID_Calc(steer_pid, steer_target, steer_feedback);
    // 输出限幅上限 +63（防止舵机右转超过机械极限）
    if(servo_output >= 63)
    {
        servo_output = 63;
    }
    // 输出限幅下限 -63（防止舵机左转超过机械极限）
    else if(servo_output <= -63)
    {
        servo_output  = -63 ;
    }
    return servo_output ;
}

/**
 * @brief 电机速度环PID控制（增量式）
 *        根据速度偏差计算电机PWM增量输出
 * @param motor_pid     PID结构体指针
 * @param speed_target  目标速度
 * @param speed_feedback 反馈速度（编码器实测）
 * @return 电机PID输出（限幅 ±20）
 *
 * 限幅说明：±20 为单次控制周期内电机PWM调整上限，
 * 防止单步加速过猛导致车轮打滑或车体失稳。
 */
float motor_pid_control(IncPID_t *motor_pid, float speed_target, float speed_feedback)
{
    // 增量式PID计算累计输出
    float motor_output = IncPID_Calc(motor_pid, speed_target, speed_feedback);

    // 输出限幅上限 +20（防止单周期加速过猛）
    if(motor_output >= 20)
    {
        motor_output = 20;
    }
    // 输出限幅下限 -20（防止单周期减速/反转过猛）
    else if(motor_output <= -20)
    {
        motor_output = -20;
    }
    return motor_output;
}
