/**
 * @file smartcar_app.c
 * @brief Smart car application lifecycle and task table.
 * @author GS_Mark
 *
 * @par Processing flow
 * ISR entries do minimum bounded work and task processing is dispatched by
 * the cooperative scheduler. App owns lifecycle entrypoints and the application
 * task table.
 */

#include "smartcar_app.h"
#include "control.h"
#include "debug_display.h"
#include "event.h"
#include "feedback_service.h"
#include "scheduler.h"
#include "sensor.h"
#include "vision.h"

typedef struct
{
    task_fn_t              handler;
    uint32_t               period_ms;
    event_mask_t           trigger;
    scheduler_task_phase_t phase;
} smartcar_task_desc_t;

static void SensorTask_Gyro10ms(event_mask_t events);
static void SensorTask_Encoder50ms(event_mask_t events);
static void VisionTask_OnFrame(event_mask_t events);
static void ControlTask_10ms(event_mask_t events);
static void FeedbackTask_10ms(event_mask_t events);
static void DiagnosticsTask_100ms(event_mask_t events);

static const smartcar_task_desc_t s_smartcar_tasks[] =
{
    {
        .handler = SensorTask_Gyro10ms,
        .period_ms = 0U,
        .trigger = EVT_GYRO_10MS,
        .phase = SCHEDULER_TASK_PHASE_SENSOR_EVENT
    },
    {
        .handler = SensorTask_Encoder50ms,
        .period_ms = 0U,
        .trigger = EVT_ENCODER_50MS,
        .phase = SCHEDULER_TASK_PHASE_SENSOR_EVENT
    },
    {
        .handler = VisionTask_OnFrame,
        .period_ms = 0U,
        .trigger = EVT_CAM_FRAME,
        .phase = SCHEDULER_TASK_PHASE_NORMAL_EVENT
    },
    {
        .handler = ControlTask_10ms,
        .period_ms = 10U,
        .trigger = EVT_NONE,
        .phase = SCHEDULER_TASK_PHASE_FAST_PERIODIC
    },
    {
        .handler = FeedbackTask_10ms,
        .period_ms = 10U,
        .trigger = EVT_NONE,
        .phase = SCHEDULER_TASK_PHASE_FAST_PERIODIC
    },
    {
        .handler = DiagnosticsTask_100ms,
        .period_ms = 100U,
        .trigger = EVT_NONE,
        .phase = SCHEDULER_TASK_PHASE_SLOW_PERIODIC
    },
};

static const uint8_t s_smartcar_task_count =
    (uint8_t)(sizeof(s_smartcar_tasks) / sizeof(s_smartcar_tasks[0]));

/**
 * @brief 处理 10ms 陀螺仪采样事件。
 *
 * @param[in] events 本轮调度事件掩码。
 * @return void。
 */
static void SensorTask_Gyro10ms(event_mask_t events)
{
    if ((events & EVT_GYRO_10MS) == 0U)
    {
        return;
    }

    SensorService_ProcessGyro10ms();
}

/**
 * @brief 处理 50ms 编码器窗口事件。
 *
 * @param[in] events 本轮调度事件掩码。
 * @return void。
 */
static void SensorTask_Encoder50ms(event_mask_t events)
{
    if ((events & EVT_ENCODER_50MS) == 0U)
    {
        return;
    }

    SensorService_ProcessEncoder50ms();
}

/**
 * @brief 处理摄像头新帧事件。
 *
 * Steps:
 *   1. 确认事件和摄像头 ready 标志。
 *   2. 执行视觉处理和元素检测。
 *   3. 通知反馈服务并清除摄像头帧标志。
 *
 * @param[in] events 本轮调度事件掩码。
 * @return void。
 */
static void VisionTask_OnFrame(event_mask_t events)
{
    vision_track_element_t element = VISION_TRACK_ELEMENT_NONE;

    if ((events & EVT_CAM_FRAME) == 0U)
    {
        return;
    }

    if (!Vision_IsFrameReady())
    {
        return;
    }

    Vision_Process();
    element = Vision_DetectElement();
    FeedbackService_NotifyTrackElement(element);

    Vision_MarkFrameConsumed();
}

/**
 * @brief 执行 10ms 控制和执行器输出任务。
 *
 * @param[in] events 本轮调度事件掩码，当前未使用。
 * @return void。
 */
static void ControlTask_10ms(event_mask_t events)
{
    (void)events;

    Control_Update();
    Control_ApplyActuator();
}

/**
 * @brief 推进 10ms 反馈时序任务。
 *
 * @param[in] events 本轮调度事件掩码，当前未使用。
 * @return void。
 */
static void FeedbackTask_10ms(event_mask_t events)
{
    (void)events;

    FeedbackService_Tick();
}

/**
 * @brief 执行 100ms 诊断显示任务。
 *
 * @param[in] events 本轮调度事件掩码。
 * @return void。
 */
static void DiagnosticsTask_100ms(event_mask_t events)
{
    DebugDisplayService_Update(events);
}

/**
 * @brief 应用层初始化入口。
 *
 * Steps:
 *   1. 注册应用任务表中的业务任务。
 *
 * @return void。
 */
void SmartcarApp_RegisterTasks(void)
{
    uint8_t i = 0U;

    for (i = 0U; i < s_smartcar_task_count; i++)
    {
        (void)Scheduler_AddEx(s_smartcar_tasks[i].handler,
                              s_smartcar_tasks[i].period_ms,
                              s_smartcar_tasks[i].trigger,
                              s_smartcar_tasks[i].phase);
    }
}
