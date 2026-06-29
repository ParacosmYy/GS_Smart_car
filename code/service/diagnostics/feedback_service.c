/**
 * @file feedback_service.c
 * @brief 行驶反馈服务。
 * @author GS_Mark
 *
 * @par 设计说明
 * 反馈服务作为诊断入口，维护赛道元素到蜂鸣器事件的业务映射，并通过
 * 执行器 BSP facade 触发非阻塞反馈。
 */

#include "feedback_service.h"
#include "actuator.h"

/**
 * @brief 将赛道元素映射为蜂鸣器反馈事件。
 *
 * Steps:
 *   1. 将无元素和未知元素保持为 BUZZER_EVENT_NONE。
 *   2. 将圆环和十字路口元素映射为对应蜂鸣器事件。
 *
 * @param[in] element 视觉元素 ID。
 * @return 蜂鸣器反馈事件。
 */
static buzzer_event_t FeedbackService_MapTrackElement(vision_track_element_t element)
{
    buzzer_event_t event = BUZZER_EVENT_NONE;

    switch (element)
    {
    case VISION_TRACK_ELEMENT_NONE:
        event = BUZZER_EVENT_NONE;
        break;

    case VISION_TRACK_ELEMENT_RING:
        event = BUZZER_EVENT_RING;
        break;

    case VISION_TRACK_ELEMENT_CROSSROAD:
        event = BUZZER_EVENT_CROSSROAD;
        break;

    default:
        event = BUZZER_EVENT_NONE;
        break;
    }

    return event;
}

/**
 * @brief 通知赛道元素检测结果。
 *
 * Steps:
 *   1. 将视觉元素映射为蜂鸣器事件。
 *   2. 直接触发执行器 facade，由 Buzzer 保持忙时排队和优先级语义。
 *
 * @param[in] element 视觉元素 ID。
 * @return void。
 */
void FeedbackService_NotifyTrackElement(vision_track_element_t element)
{
    buzzer_event_t event = FeedbackService_MapTrackElement(element);

    Actuator_TriggerBuzzer(event);
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
