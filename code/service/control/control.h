#ifndef CODE_CONTROL_CONTROL_H_
#define CODE_CONTROL_CONTROL_H_

/**
 * @file control.h
 * @brief 控制决策与执行输出服务接口。
 */

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
 * @brief 初始化控制 Handler。
 *
 * 初始化 PID、清零控制输出，并在首次控制更新前保持丢线保护状态。
 */
void Control_Init(void);

/**
 * @brief 更新控制输出快照。
 *
 * 读取：视觉快照、传感器服务快照。
 * 写入：控制 Handler 内部输出快照和丢线保护缓存。
 * 当视觉丢线计数超过 LOST_LINE_THRESHOLD 时直接返回，不做计算。
 */
void Control_Update(void);

/**
 * @brief 应用最近一次控制输出到物理执行器。
 *
 * 丢线状态来自最近一次 Control_Update 缓存。
 * 丢线时：舵机回中位，电机保持当前执行器命令。
 * 正常时：舵机和电机均由 PID 输出驱动。
 */
void Control_ApplyActuator(void);

/**
 * @brief 获取控制输出快照。
 *
 * @param[out] p_output 控制输出快照；为空时不写入。
 */
void Control_GetOutputSnapshot(control_output_t *p_output);

#ifdef __cplusplus
}
#endif

#endif /* CODE_CONTROL_CONTROL_H_ */
