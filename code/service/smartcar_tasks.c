/**
 * @file smartcar_tasks.c
 * @brief 业务任务编排。
 */

#include "smartcar_tasks.h"

#include "control.h"
#include "debug_display.h"
#include "feedback_service.h"
#include "platform/interface/device_if.h"
#include "sensor.h"
#include "vision.h"

void SensorTask_Gyro10ms(event_mask_t events)
{
    if ((events & EVT_GYRO_10MS) == 0U)
    {
        return;
    }

    SensorService_ProcessGyro10ms();
}

void SensorTask_Encoder50ms(event_mask_t events)
{
    if ((events & EVT_ENCODER_50MS) == 0U)
    {
        return;
    }

    SensorService_ProcessEncoder50ms();
}

void VisionTask_OnFrame(event_mask_t events)
{
    uint8_t element = 0U;

    if ((events & EVT_CAM_FRAME) == 0U)
    {
        return;
    }

    if (!Device_CameraReady())
    {
        return;
    }

    Vision_Process();
    element = Vision_DetectElement();
    FeedbackService_NotifyTrackElement(element);

    Device_CameraClear();
}

void ControlTask_10ms(event_mask_t events)
{
    (void)events;

    Control_Update();
    Actuator_Apply();
}

void FeedbackTask_Tick(event_mask_t events)
{
    (void)events;

    FeedbackService_Tick();
}

void DiagnosticsTask_100ms(event_mask_t events)
{
    DebugDisplayService_Update(events);
    DebugDisplayService_DrawVisionLines();
}
