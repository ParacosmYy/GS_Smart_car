#ifndef CODE_BSP_ACTUATOR_H_
#define CODE_BSP_ACTUATOR_H_

/**
 * @file actuator.h
 * @brief 执行器 BSP 语义 facade 接口。
 * @author GS_Mark
 */

#include <stdint.h>

#include "motor.h"
#include "servo.h"
#include "buzzer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 设置转向舵机相对中位偏移。 */
static inline void Actuator_SetServo(int32_t offset)
{
    Servo_SetAngle(offset);
}

/** @brief 设置左电机速度命令。 */
static inline void Actuator_SetMotorLeft(int32_t speed)
{
    Motor_SetLeft(speed);
}

/** @brief 设置右电机速度命令。 */
static inline void Actuator_SetMotorRight(int32_t speed)
{
    Motor_SetRight(speed);
}

/** @brief 推进执行器反馈 10ms tick。 */
static inline void Actuator_TickFeedback(void)
{
    Buzzer_Tick();
}

/** @brief 查询蜂鸣器反馈状态。 */
static inline uint8_t Actuator_IsBuzzerBusy(void)
{
    return Buzzer_IsBusy();
}

/** @brief 触发蜂鸣器反馈事件。 */
static inline void Actuator_TriggerBuzzer(buzzer_event_t event)
{
    Buzzer_Trigger(event);
}

#ifdef __cplusplus
}
#endif

#endif /* CODE_BSP_ACTUATOR_H_ */
