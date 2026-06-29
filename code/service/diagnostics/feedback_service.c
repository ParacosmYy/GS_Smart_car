/**
 * @file feedback_service.c
 * @brief 行驶反馈服务。
 */

#include "feedback_service.h"
#include "platform/interface/service_port_if.h"

void FeedbackService_NotifyTrackElement(uint8_t element)
{
    if ((element == 0U) || (Feedback_IsBusy() != 0U))
    {
        return;
    }

    Feedback_NotifyElement(element);
}

void FeedbackService_Tick(void)
{
    Feedback_Tick();
}
