#ifndef CODE_SERVICE_DIAGNOSTICS_FEEDBACK_SERVICE_H_
#define CODE_SERVICE_DIAGNOSTICS_FEEDBACK_SERVICE_H_

/**
 * @file feedback_service.h
 * @brief 行驶反馈服务接口。
 */

#include "vision.h"

/** @brief 根据视觉元素触发提示 */
void FeedbackService_NotifyTrackElement(vision_track_element_t element);

/** @brief 推进非阻塞反馈时序 */
void FeedbackService_Tick(void);

#endif /* CODE_SERVICE_DIAGNOSTICS_FEEDBACK_SERVICE_H_ */
