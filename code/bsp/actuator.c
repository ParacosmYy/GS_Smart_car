/**
 * @file actuator.c
 * @brief 执行器 BSP 语义 facade 实现。
 * @author GS_Mark
 */

#include "actuator.h"

#include "buzzer.h"
#include "motor.h"
#include "servo.h"

/**
 * @brief 将执行器反馈事件映射为蜂鸣器事件。
 *
 * @param[in] event 执行器反馈事件。
 * @return 蜂鸣器事件。
 */
static buzzer_event_t Actuator_MapFeedbackEvent(actuator_feedback_event_t event)
{
    buzzer_event_t buzzer_event = BUZZER_EVENT_NONE;

    switch (event)
    {
    case ACTUATOR_FEEDBACK_NONE:
        buzzer_event = BUZZER_EVENT_NONE;
        break;

    case ACTUATOR_FEEDBACK_RING:
        buzzer_event = BUZZER_EVENT_RING;
        break;

    case ACTUATOR_FEEDBACK_CROSSROAD:
        buzzer_event = BUZZER_EVENT_CROSSROAD;
        break;

    default:
        buzzer_event = BUZZER_EVENT_NONE;
        break;
    }

    return buzzer_event;
}

/**
 * @brief 设置转向舵机相对中位偏移。
 *
 * @param[in] offset 舵机相对中位偏移。
 * @return void。
 */
void Actuator_SetServo(int32_t offset)
{
    Servo_SetAngle(offset);
}

/**
 * @brief 设置左电机速度命令。
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
 * @param[in] speed 右电机速度命令。
 * @return void。
 */
void Actuator_SetMotorRight(int32_t speed)
{
    Motor_SetRight(speed);
}

/**
 * @brief 推进执行器反馈时序。
 *
 * @return void。
 */
void Actuator_TickFeedback(void)
{
    Buzzer_Tick();
}

/**
 * @brief 触发执行器反馈事件。
 *
 * @param[in] event 执行器反馈事件。
 * @return void。
 */
void Actuator_TriggerFeedback(actuator_feedback_event_t event)
{
    Buzzer_Trigger(Actuator_MapFeedbackEvent(event));
}
