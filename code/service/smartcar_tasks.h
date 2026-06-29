#ifndef CODE_SERVICE_SMARTCAR_TASKS_H_
#define CODE_SERVICE_SMARTCAR_TASKS_H_

/**
 * @file smartcar_tasks.h
 * @brief 业务任务入口。
 */

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

void SensorTask_Gyro10ms(event_mask_t events);
void SensorTask_Encoder50ms(event_mask_t events);
void VisionTask_OnFrame(event_mask_t events);
void ControlTask_10ms(event_mask_t events);
void FeedbackTask_Tick(event_mask_t events);
void DiagnosticsTask_100ms(event_mask_t events);

#ifdef __cplusplus
}
#endif

#endif /* CODE_SERVICE_SMARTCAR_TASKS_H_ */
