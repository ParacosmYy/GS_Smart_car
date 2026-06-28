/**
 * @file feedback_service.c
 * @brief 行驶反馈服务实现。
 *
 * 本服务把 App 层的“赛道元素提示”语义转换为 BSP 蜂鸣器模式，
 * 避免 App 直接依赖板级蜂鸣器驱动。
 */

#include "feedback_service.h"

#include "buzzer.h"

/**
 * @brief 根据视觉元素触发提示。
 *
 * 处理步骤：
 *  1. 忽略无元素或蜂鸣器忙状态。
 *  2. 圆环触发短促提示，十字路口触发长提示。
 *
 * @param[in] element : 视觉元素编号，0=无，1=圆环，2=十字路口。
 *
 * @return void : 无返回值。
 *
 * */
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
    else
    {
        /* 预留其他视觉元素，不触发提示。 */
    }
}

/**
 * @brief 推进非阻塞反馈时序。
 *
 * 处理步骤：
 *  1. 委托 BSP 蜂鸣器推进当前提示模式。
 *
 * @return void : 无返回值。
 *
 * */
void FeedbackService_Tick(void)
{
    Buzzer_Tick();
}
