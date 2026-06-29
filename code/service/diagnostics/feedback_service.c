/**
 * @file feedback_service.c
 * @brief 行驶反馈服务。
 * @author GS_Mark
 *
 * @par 设计说明
 * 反馈服务屏蔽蜂鸣器忙闲判断和元素到提示音的映射，Service 任务只关心何时 tick。
 */

#include "feedback_service.h"
#include "buzzer.h"

/**
 * @brief 通知赛道元素检测结果。
 *
 * Steps:
 *   1. 忽略空元素。
 *   2. 反馈端口忙时不抢占当前提示。
 *   3. 空闲时转发元素事件。
 *
 * @param[in] element 视觉元素 ID。
 * @return void。
 */
void FeedbackService_NotifyTrackElement(uint8_t element)
{
    if ((element == 0U) || (Buzzer_IsBusy() != 0U))
    {
        return;
    }

    if (element == 1U)
    {
        Buzzer_Trigger(BUZZER_EVENT_RING);
    }
    else if (element == 2U)
    {
        Buzzer_Trigger(BUZZER_EVENT_CROSSROAD);
    }
}

/**
 * @brief 推进反馈服务 10ms tick。
 *
 * @return void。
 */
void FeedbackService_Tick(void)
{
    Buzzer_Tick();
}
