/*
 * control.c
 *
 * 控制决策 + 执行输出层。
 * 从 image.c 中拆分，将视觉处理与控制逻辑解耦。
 */

#include "control.h"
#include "zf_common_headfile.h"
#include "config.h"

void Control_Update(void)
{
    // 丢线帧数过多，停止控制更新，避免误判导致冲出赛道
    if(lost_count > LOST_LINE_THRESHOLD)
    {
        return;
    }
    // 舵机：位置式PID，目标居中(0)，反馈为视觉中线偏差 calculate_error
    servo_pid_output = servo_pid_contorl(&servo_pid, 0, calculate_error);
    // 左电机：增量式PID，反馈为编码器实测速度 left_encoder_speed
    left_motor_pid_output = motor_pid_control(&left_motor_pid, 0, left_encoder_speed);
    // 右电机：增量式PID，反馈为编码器实测速度 right_encoder_speed
    right_motor_pid_output = motor_pid_control(&right_motor_pid, 0, right_encoder_speed);
}

void Actuator_Apply(void)
{
    // 丢线保护：舵机回正中位 SERVO_CENTER_DUTY，避免继续转向冲出赛道
    if(lost_count > LOST_LINE_THRESHOLD)
    {
        pwm_set_duty(ATOM1_CH1_P33_9, SERVO_CENTER_DUTY);
        return;
    }
    // 舵机PWM = 中心值 + PID增量输出（关键修复：原代码此行被注释导致舵机不响应）
    pwm_set_duty(ATOM1_CH1_P33_9, SERVO_CENTER_DUTY + (int)servo_pid_output);
    // 左右电机施加PID速度输出
    motor_setspeed_left((int32)left_motor_pid_output);
    motor_setspeed_right((int32)right_motor_pid_output);
}
