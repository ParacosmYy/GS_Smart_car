#ifndef CODE_BSP_ACTUATOR_H_
#define CODE_BSP_ACTUATOR_H_

/**
 * @file actuator.h
 * @brief 执行器 BSP 语义 facade 接口。
 * @author GS_Mark
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 设置转向舵机相对中位偏移。 */
void Actuator_SetServo(int32_t offset);

/** @brief 设置左电机速度命令。 */
void Actuator_SetMotorLeft(int32_t speed);

/** @brief 设置右电机速度命令。 */
void Actuator_SetMotorRight(int32_t speed);

/** @brief 根据视觉赛道元素触发行驶反馈。 */
void Actuator_NotifyTrackElement(uint8_t element);

/** @brief 推进执行器反馈 10ms tick。 */
void Actuator_TickFeedback(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_BSP_ACTUATOR_H_ */
