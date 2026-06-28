/*
 * control.c
 *
 * 控制决策 + 执行输出层。
 * 从 image.c 中拆分，将视觉处理与控制逻辑解耦。
 */

#include "control.h"
#include "config.h"
#include "pid.h"
#include "sensor.h"
#include "vision.h"
#include "platform/interface/actuator_if.h"

//******************************** Types ************************************//
typedef struct
{
    PosPID_t servo_pid;
    IncPID_t left_motor_pid;
    IncPID_t right_motor_pid;
    control_output_t output;
} control_handler_t;
//******************************** Types ************************************//

//******************************** Variables ********************************//
static control_handler_t s_control_handler;
//******************************** Variables ********************************//

//******************************** Declaring ********************************//
static uint8_t Control_IsLostLine(uint8_t lost_count);
//******************************** Declaring ********************************//

//******************************** Implement ********************************//
/**
 * @brief 判断视觉丢线是否超过保护阈值。
 *
 * @param[in] lost_count : 连续丢线计数。
 *
 * @return uint8_t : 1 表示需要进入丢线保护，0 表示正常控制。
 *
 * */
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
 * 处理步骤：
 *  1. 使用 config.h 中的参数初始化舵机和左右电机 PID。
 *  2. 清零控制输出快照，避免上电后残留输出。
 *
 * @return void : 无返回值。
 *
 * */
void Control_Init(void)
{
    PosPID_Init(&s_control_handler.servo_pid, SERVO_PID_KP, SERVO_PID_KI, SERVO_PID_KD);
    IncPID_Init(&s_control_handler.left_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
    IncPID_Init(&s_control_handler.right_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);

    s_control_handler.output.servo = 0.0f;
    s_control_handler.output.left_motor = 0.0f;
    s_control_handler.output.right_motor = 0.0f;
}

void Control_Update(void)
{
    vision_control_snapshot_t vision_snapshot = {0};

    Vision_GetControlSnapshot(&vision_snapshot);

    // 丢线帧数过多，停止控制更新，避免误判导致冲出赛道
    if (Control_IsLostLine(vision_snapshot.lost_count) != 0U)
    {
        return;
    }

    // 舵机：位置式 PID，目标居中(0)，反馈为视觉中线偏差。
    s_control_handler.output.servo = ServoPid_Control(&s_control_handler.servo_pid,
                                                      0,
                                                      vision_snapshot.calculate_error);
    // 左电机：增量式PID，反馈为编码器实测速度
    s_control_handler.output.left_motor = MotorPid_Control(&s_control_handler.left_motor_pid,
                                                           0,
                                                           SensorService_GetLeftEncoderSpeed());
    // 右电机：增量式PID，反馈为编码器实测速度
    s_control_handler.output.right_motor = MotorPid_Control(&s_control_handler.right_motor_pid,
                                                            0,
                                                            SensorService_GetRightEncoderSpeed());
}

void Actuator_Apply(void)
{
    vision_control_snapshot_t vision_snapshot = {0};

    Vision_GetControlSnapshot(&vision_snapshot);

    // 丢线保护：舵机回正中位 SERVO_CENTER_DUTY，避免继续转向冲出赛道
    if (Control_IsLostLine(vision_snapshot.lost_count) != 0U)
    {
        Actuator_SetServo(0);
        return;
    }
    Actuator_SetServo((int32_t)s_control_handler.output.servo);
    Actuator_SetMotorLeft((int32_t)s_control_handler.output.left_motor);
    Actuator_SetMotorRight((int32_t)s_control_handler.output.right_motor);
}

/**
 * @brief 获取控制输出快照。
 *
 * @param[out] p_output : 控制输出快照。
 *
 * @return void : 无返回值。
 *
 * */
void Control_GetOutputSnapshot(control_output_t *p_output)
{
    if (p_output == 0)
    {
        return;
    }

    *p_output = s_control_handler.output;
}
//******************************** Implement ********************************//
