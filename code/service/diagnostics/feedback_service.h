#ifndef FEEDBACK_SERVICE_H_
#define FEEDBACK_SERVICE_H_

/**
 * @file feedback_service.h
 * @brief 行驶反馈服务接口。
 */

#include <stdint.h>

/** @brief 根据视觉元素触发提示 */
void FeedbackService_NotifyTrackElement(uint8_t element);

/** @brief 推进非阻塞反馈时序 */
void FeedbackService_Tick(void);

#endif /* FEEDBACK_SERVICE_H_ */
