/*
 * pid.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */
#include "pid.h"
#include "zf_common_headfile.h"

float servo_pid_output = 0.0;
float left_motor_pid_output = 0;
float right_motor_pid_output = 0;

PosPID_t servo_pid  ;
IncPID_t left_motor_pid ;
IncPID_t right_motor_pid ;

// 位置式PID初始化
void PosPID_Init(PosPID_t *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0;
    pid->prev_err = 0;
    pid->output = 0;
}

// 增量式PID初始化
void IncPID_Init(IncPID_t *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_err = 0;
    pid->prev_prev_err = 0;
    pid->output = 0;
}

// 位置式PID
float PosPID_Calc(PosPID_t *pid, float target, float feedback) {
    float err = target - feedback;
    pid->integral += err;
    float derivative = err - pid->prev_err;
    pid->output = pid->kp * err + pid->ki * pid->integral + pid->kd * derivative;
    pid->prev_err = err;
    return pid->output;
}

// 增量式PID
float IncPID_Calc(IncPID_t *pid, float target, float feedback) {
    float err = target - feedback;
    float delta_output = pid->kp * (err - pid->prev_err)
                      + pid->ki * err
                      + pid->kd * (err - 2 * pid->prev_err + pid->prev_prev_err);
    pid->output += delta_output;
    pid->prev_prev_err = pid->prev_err;
    pid->prev_err = err;
    return pid->output;
}

//单独转向环控制
float servo_pid_contorl(PosPID_t *steer_pid , float steer_target , float steer_feedback)
{
    float servo_output = PosPID_Calc(steer_pid, steer_target, steer_feedback);
    if(servo_output >= 63)
    {
        servo_output = 63;
    }
    else if(servo_output <= -63)
    {
        servo_output  = -63 ;
    }
    return servo_output ;
}

// 单独电机速度环控制（增量式PID）
float motor_pid_control(IncPID_t *motor_pid, float speed_target, float speed_feedback)
{
    float motor_output = IncPID_Calc(motor_pid, speed_target, speed_feedback);

    if(motor_output >= 20)
    {
        motor_output = 20;
    }
    else if(motor_output <= -20)
    {
        motor_output = -20;
    }
    return motor_output;
}
