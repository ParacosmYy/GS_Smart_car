/**
 * @file actuator.c
 * @brief 执行器 BSP 语义 facade 实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本模块收敛 Service 层对具体执行器驱动头文件的依赖。初始化职责仍由
 * smartcar_board.c 分别调用 Motor_Init、Servo_Init 和 Buzzer_Init 完成。
 */

#include "actuator.h"

#include "buzzer.h"
#include "motor.h"
#include "servo.h"

#define ACTUATOR_TRACK_ELEMENT_NONE       0U
#define ACTUATOR_TRACK_ELEMENT_RING       1U
#define ACTUATOR_TRACK_ELEMENT_CROSSROAD  2U

/**
 * @brief 设置转向舵机相对中位偏移。
 *
 * Steps:
 *   1. 将语义级舵机偏移转发给 Servo BSP。
 *
 * @param[in] offset 相对中位的占空比偏移。
 * @return void。
 */
void Actuator_SetServo(int32_t offset)
{
    Servo_SetAngle(offset);
}

/**
 * @brief 设置左电机速度命令。
 *
 * Steps:
 *   1. 将语义级左电机速度命令转发给 Motor BSP。
 *
 * @param[in] speed 左电机速度命令。
 * @return void。
 */
void Actuator_SetMotorLeft(int32_t speed)
{
    Motor_SetLeft(speed);
}

/**
 * @brief 设置右电机速度命令。
 *
 * Steps:
 *   1. 将语义级右电机速度命令转发给 Motor BSP。
 *
 * @param[in] speed 右电机速度命令。
 * @return void。
 */
void Actuator_SetMotorRight(int32_t speed)
{
    Motor_SetRight(speed);
}

/**
 * @brief 根据视觉赛道元素触发行驶反馈。
 *
 * Steps:
 *   1. 忽略无元素事件。
 *   2. 将圆环和十字路口语义映射到蜂鸣器 BSP 事件。
 *   3. 未识别元素保持静默，避免误触发反馈。
 *
 * @param[in] element 视觉元素 ID，0 表示无元素。
 * @return void。
 */
void Actuator_NotifyTrackElement(uint8_t element)
{
    if (element == ACTUATOR_TRACK_ELEMENT_NONE)
    {
        return;
    }

    if (element == ACTUATOR_TRACK_ELEMENT_RING)
    {
        Buzzer_Trigger(BUZZER_EVENT_RING);
    }
    else if (element == ACTUATOR_TRACK_ELEMENT_CROSSROAD)
    {
        Buzzer_Trigger(BUZZER_EVENT_CROSSROAD);
    }
}

/**
 * @brief 推进执行器反馈 10ms tick。
 *
 * Steps:
 *   1. 转发 tick 给 Buzzer BSP 的非阻塞状态机。
 *
 * @return void。
 */
void Actuator_TickFeedback(void)
{
    Buzzer_Tick();
}
