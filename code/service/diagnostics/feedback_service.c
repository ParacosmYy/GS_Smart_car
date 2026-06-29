/**
 * @file feedback_service.c
 * @brief 行驶反馈服务。
 * @author GS_Mark
 *
 * @par 设计说明
 * 反馈服务作为诊断入口，仅向执行器 BSP facade 转发元素事件和周期 tick。
 */

#include "feedback_service.h"
#include "actuator.h"

/**
 * @brief 通知赛道元素检测结果。
 *
 * Steps:
 *   1. 转发元素事件，由执行器 BSP facade 保持反馈映射与非阻塞排队语义。
 *
 * @param[in] element 视觉元素 ID。
 * @return void。
 */
void FeedbackService_NotifyTrackElement(uint8_t element)
{
    Actuator_NotifyTrackElement(element);
}

/**
 * @brief 推进反馈服务 10ms tick。
 *
 * @return void。
 */
void FeedbackService_Tick(void)
{
    Actuator_TickFeedback();
}
