/**
 * @file smartcar_tasks.c
 * @brief 业务任务编排。
 */

#include "smartcar_tasks.h"

#include "control.h"
#include "debug_display.h"
#include "event.h"
#include "feedback_service.h"
#include "platform/interface/device_if.h"
#include "scheduler.h"
#include "sensor.h"
#include "vision.h"

typedef struct
{
    task_fn_t    handler;
    uint32_t     period_ms;
    event_mask_t trigger;
} smartcar_task_desc_t;

static void SensorTask_Gyro10ms(event_mask_t events);
static void SensorTask_Encoder50ms(event_mask_t events);
static void VisionTask_OnFrame(event_mask_t events);
static void ControlTask_10ms(event_mask_t events);
static void FeedbackTask_10ms(event_mask_t events);
static void DiagnosticsTask_100ms(event_mask_t events);
static uint8_t SmartcarTasks_RegisterOne(const smartcar_task_desc_t *p_task);

static const smartcar_task_desc_t s_smartcar_tasks[] =
{
    { .handler = SensorTask_Gyro10ms,    .period_ms = 0U,   .trigger = EVT_GYRO_10MS },
    { .handler = SensorTask_Encoder50ms, .period_ms = 0U,   .trigger = EVT_ENCODER_50MS },
    { .handler = VisionTask_OnFrame,     .period_ms = 0U,   .trigger = EVT_CAM_FRAME },
    { .handler = ControlTask_10ms,       .period_ms = 10U,  .trigger = EVT_NONE },
    { .handler = FeedbackTask_10ms,      .period_ms = 10U,  .trigger = EVT_NONE },
    { .handler = DiagnosticsTask_100ms,  .period_ms = 100U, .trigger = EVT_NONE },
};

static const uint8_t s_smartcar_task_count =
    (uint8_t)(sizeof(s_smartcar_tasks) / sizeof(s_smartcar_tasks[0]));

static void SensorTask_Gyro10ms(event_mask_t events)
{
    if ((events & EVT_GYRO_10MS) == 0U)
    {
        return;
    }

    SensorService_ProcessGyro10ms();
}

static void SensorTask_Encoder50ms(event_mask_t events)
{
    if ((events & EVT_ENCODER_50MS) == 0U)
    {
        return;
    }

    SensorService_ProcessEncoder50ms();
}

static void VisionTask_OnFrame(event_mask_t events)
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

static void ControlTask_10ms(event_mask_t events)
{
    (void)events;

    Control_Update();
    Actuator_Apply();
}

static void FeedbackTask_10ms(event_mask_t events)
{
    (void)events;

    FeedbackService_Tick();
}

static void DiagnosticsTask_100ms(event_mask_t events)
{
    DebugDisplayService_Update(events);
    DebugDisplayService_DrawVisionLines();
}

static uint8_t SmartcarTasks_RegisterOne(const smartcar_task_desc_t *p_task)
{
    int8_t task_index = -1;
    uint8_t failed = 0U;

    if (p_task == 0)
    {
        return 1U;
    }

    task_index = scheduler_add(p_task->handler, p_task->period_ms, p_task->trigger);
    if (task_index < 0)
    {
        failed = 1U;
    }

    return failed;
}

uint8_t SmartcarTasks_RegisterAll(void)
{
    uint8_t fail_count = 0U;
    uint8_t i = 0U;

    for (i = 0U; i < s_smartcar_task_count; i++)
    {
        fail_count += SmartcarTasks_RegisterOne(&s_smartcar_tasks[i]);
    }

    return fail_count;
}
