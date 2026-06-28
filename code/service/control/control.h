#ifndef CODE_CONTROL_CONTROL_H_
#define CODE_CONTROL_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 控制更新
 *        根据视觉偏差与编码器速度计算舵机和电机的PID输出
 *
 * 读取：calculate_error（视觉）、left/right_encoder_speed（中断采样）。
 * 写入：servo_pid_output、left_motor_pid_output、right_motor_pid_output。
 * 当 lost_count 超过 LOST_LINE_THRESHOLD 时直接返回，不做计算。
 */
void Control_Update(void);

/**
 * @brief 执行器输出
 *        将PID计算结果应用到物理执行器（舵机PWM + 电机速度）
 *
 * 丢线时：舵机回中位，电机保持当前速度。
 * 正常时：舵机由PID驱动，电机由PID驱动。
 */
void Actuator_Apply(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_CONTROL_CONTROL_H_ */
