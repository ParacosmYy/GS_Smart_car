#ifndef CODE_CONTROL_CONTROL_H_
#define CODE_CONTROL_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float servo;        // 舵机 PID 输出
    float left_motor;   // 左电机 PID 输出
    float right_motor;  // 右电机 PID 输出
} control_output_t;

/**
 * @brief 初始化控制 Handler
 */
void Control_Init(void);

/**
 * @brief 控制更新
 *        根据视觉偏差与编码器速度计算舵机和电机的PID输出
 *
 * 读取：视觉快照、传感器服务的左右编码器速度。
 * 写入：控制 Handler 内部输出快照。
 * 当视觉丢线计数超过 LOST_LINE_THRESHOLD 时直接返回，不做计算。
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

/**
 * @brief 获取控制输出快照
 */
void Control_GetOutputSnapshot(control_output_t *p_output);

#ifdef __cplusplus
}
#endif

#endif /* CODE_CONTROL_CONTROL_H_ */
