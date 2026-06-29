/**
 * @file control.c
 * @brief 控制决策与执行输出服务实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 控制服务读取视觉和传感器快照，计算舵机与左右电机输出，再通过执行器端口应用。
 */

#include "control.h"
#include "config.h"
#include "motor.h"
#include "pid.h"
#include "sensor.h"
#include "servo.h"
#include "vision.h"

typedef struct
{
    PosPID_t servo_pid;
    IncPID_t left_motor_pid;
    IncPID_t right_motor_pid;
    control_output_t output;
} control_handler_t;

static control_handler_t s_control_handler;

/**
 * @brief 判断视觉丢线是否超过保护阈值。
 *
 * @param[in] lost_count 连续丢线计数。
 * @return 1 表示需要进入丢线保护；0 表示正常控制。
 */
static uint8_t Control_IsLostLine(uint8_t lost_count)
{
    uint8_t is_lost = 0U;

    if (lost_count > LOST_LINE_THRESHOLD)
    {
        is_lost = 1U;
    }

    return is_lost;
}

/**
 * @brief 初始化控制 Handler。
 *
 * Steps:
 *   1. 使用 config.h 中的参数初始化舵机和左右电机 PID。
 *   2. 清零控制输出快照，避免上电后残留输出。
 *
 * @return void。
 */
void Control_Init(void)
{
    PosPID_Init(&s_control_handler.servo_pid, SERVO_PID_KP, SERVO_PID_KI, SERVO_PID_KD);
    IncPID_Init(&s_control_handler.left_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
    IncPID_Init(&s_control_handler.right_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);

    s_control_handler.output.servo = 0.0f;
    s_control_handler.output.left_motor = 0.0f;
    s_control_handler.output.right_motor = 0.0f;
}

/**
 * @brief 更新控制输出快照。
 *
 * Steps:
 *   1. 读取视觉控制快照。
 *   2. 丢线超过阈值时停止更新，保留上一轮输出。
 *   3. 用视觉误差计算舵机输出。
 *   4. 用左右编码器速度计算电机增量输出。
 *
 * @return void。
 */
void Control_Update(void)
{
    vision_control_snapshot_t vision_snapshot = {0};

    Vision_GetControlSnapshot(&vision_snapshot);

    if (Control_IsLostLine(vision_snapshot.lost_count) != 0U)
    {
        return;
    }

    s_control_handler.output.servo = ServoPid_Control(&s_control_handler.servo_pid,
                                                      0,
                                                      vision_snapshot.calculate_error);
    s_control_handler.output.left_motor = MotorPid_Control(&s_control_handler.left_motor_pid,
                                                           0,
                                                           SensorService_GetLeftEncoderSpeed());
    s_control_handler.output.right_motor = MotorPid_Control(&s_control_handler.right_motor_pid,
                                                            0,
                                                            SensorService_GetRightEncoderSpeed());
}

/**
 * @brief 将控制输出应用到执行器。
 *
 * Steps:
 *   1. 读取视觉丢线状态。
 *   2. 丢线时舵机回中并跳过电机更新。
 *   3. 正常时将舵机和左右电机输出写入执行器端口。
 *
 * @return void。
 */
void Actuator_Apply(void)
{
    vision_control_snapshot_t vision_snapshot = {0};

    Vision_GetControlSnapshot(&vision_snapshot);

    if (Control_IsLostLine(vision_snapshot.lost_count) != 0U)
    {
        Servo_SetAngle(0);
        return;
    }

    Servo_SetAngle((int32_t)s_control_handler.output.servo);
    Motor_SetLeft((int32_t)s_control_handler.output.left_motor);
    Motor_SetRight((int32_t)s_control_handler.output.right_motor);
}

/**
 * @brief 获取控制输出快照。
 *
 * Steps:
 *   1. 校验输出指针。
 *   2. 复制当前控制输出。
 *
 * @param[out] p_output 控制输出快照。
 * @return void。
 */
void Control_GetOutputSnapshot(control_output_t *p_output)
{
    if (p_output == 0)
    {
        return;
    }

    *p_output = s_control_handler.output;
}
