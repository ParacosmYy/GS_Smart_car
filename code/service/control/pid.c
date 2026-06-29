/**
 * @file pid.c
 * @brief PID 算法实现。
 * @author Paracosm
 *
 * @par 设计说明
 * 本模块提供位置式 PID、增量式 PID，以及舵机和电机控制专用的限幅包装函数。
 */

#include "pid.h"

#define POS_PID_INTEGRAL_LIMIT  1000.0f
#define SERVO_PID_OUTPUT_LIMIT  63.0f
#define MOTOR_PID_OUTPUT_LIMIT  20.0f

/**
 * @brief 对浮点控制量做对称限幅。
 *
 * Steps:
 *   1. 将正向值限制在 limit 以内。
 *   2. 将反向值限制在 -limit 以内。
 *   3. 未越界时保持原值。
 *
 * @param[in] value 输入控制量。
 * @param[in] limit 控制量绝对值上限。
 * @return 限幅后的控制量。
 */
static float pid_clamp_symmetric(float value, float limit)
{
    float clamped = value;

    if (value > limit)
    {
        clamped = limit;
    }
    else if (value < -limit)
    {
        clamped = -limit;
    }

    return clamped;
}

/**
 * @brief 初始化位置式 PID 参数。
 *
 * Steps:
 *   1. 写入 Kp、Ki、Kd。
 *   2. 清零积分项、上次误差和输出。
 *
 * @param[in,out] pid PID 结构体指针。
 * @param[in] kp 比例系数。
 * @param[in] ki 积分系数。
 * @param[in] kd 微分系数。
 * @return void。
 */
void PosPID_Init(PosPID_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_err = 0.0f;
    pid->output = 0.0f;
}

/**
 * @brief 初始化增量式 PID 参数。
 *
 * Steps:
 *   1. 写入 Kp、Ki、Kd。
 *   2. 清零两级历史误差和累计输出。
 *
 * @param[in,out] pid PID 结构体指针。
 * @param[in] kp 比例系数。
 * @param[in] ki 积分系数。
 * @param[in] kd 微分系数。
 * @return void。
 */
void IncPID_Init(IncPID_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_err = 0.0f;
    pid->prev_prev_err = 0.0f;
    pid->output = 0.0f;
}

/**
 * @brief 执行位置式 PID 计算。
 *
 * Steps:
 *   1. 计算目标值与反馈值的当前误差。
 *   2. 累加积分项并做 anti-windup 限幅。
 *   3. 根据上次误差计算微分项。
 *   4. 计算并保存本次输出。
 *
 * @param[in,out] pid PID 结构体指针。
 * @param[in] target 目标值。
 * @param[in] feedback 反馈值。
 * @return PID 输出值。
 */
float PosPID_Calc(PosPID_t *pid, float target, float feedback)
{
    float err = target - feedback;
    float derivative = 0.0f;

    pid->integral += err;
    pid->integral = pid_clamp_symmetric(pid->integral, POS_PID_INTEGRAL_LIMIT);
    derivative = err - pid->prev_err;
    pid->output = pid->kp * err + pid->ki * pid->integral + pid->kd * derivative;
    pid->prev_err = err;

    return pid->output;
}

/**
 * @brief 执行增量式 PID 计算。
 *
 * Steps:
 *   1. 计算当前误差。
 *   2. 根据 err、prev_err、prev_prev_err 计算输出增量。
 *   3. 将增量累加到输出。
 *   4. 滚动更新历史误差。
 *
 * @param[in,out] pid PID 结构体指针。
 * @param[in] target 目标值。
 * @param[in] feedback 反馈值。
 * @return PID 累计输出值。
 */
float IncPID_Calc(IncPID_t *pid, float target, float feedback)
{
    float err = target - feedback;
    float delta_output = pid->kp * (err - pid->prev_err)
                       + pid->ki * err
                       + pid->kd * (err - 2.0f * pid->prev_err + pid->prev_prev_err);

    pid->output += delta_output;
    pid->prev_prev_err = pid->prev_err;
    pid->prev_err = err;

    return pid->output;
}

/**
 * @brief 计算舵机位置式 PID 输出并限幅。
 *
 * Steps:
 *   1. 使用位置式 PID 计算原始舵机输出。
 *   2. 将输出限制在 SERVO_PID_OUTPUT_LIMIT 范围内。
 *
 * @param[in,out] steer_pid 舵机 PID 结构体指针。
 * @param[in] steer_target 舵机目标值。
 * @param[in] steer_feedback 舵机反馈值，通常为视觉中线误差。
 * @return 限幅后的舵机输出。
 */
float ServoPid_Control(PosPID_t *steer_pid, float steer_target, float steer_feedback)
{
    float servo_output = PosPID_Calc(steer_pid, steer_target, steer_feedback);

    servo_output = pid_clamp_symmetric(servo_output, SERVO_PID_OUTPUT_LIMIT);

    return servo_output;
}

/**
 * @brief 计算电机增量式 PID 输出并限幅。
 *
 * Steps:
 *   1. 使用增量式 PID 计算累计电机输出。
 *   2. 将输出限制在 MOTOR_PID_OUTPUT_LIMIT 范围内。
 *
 * @param[in,out] motor_pid 电机 PID 结构体指针。
 * @param[in] speed_target 目标速度。
 * @param[in] speed_feedback 编码器反馈速度。
 * @return 限幅后的电机输出。
 */
float MotorPid_Control(IncPID_t *motor_pid, float speed_target, float speed_feedback)
{
    float motor_output = IncPID_Calc(motor_pid, speed_target, speed_feedback);

    motor_output = pid_clamp_symmetric(motor_output, MOTOR_PID_OUTPUT_LIMIT);

    return motor_output;
}
