/*
 * smartcar_app.c
 * @brief Smart car application scheduler implementation.
 *
 * Processing flow:
 * ISR entries do minimum bounded work and task processing is dispatched by
 * the cooperative scheduler:
 *    SmartcarApp_TaskGyro      陀螺仪传感器服务处理（事件触发）
 *    SmartcarApp_TaskEncoder   编码器传感器服务处理（事件触发）
 *    SmartcarApp_TaskVision    视觉处理 + 元素检测 + 反馈服务（DMA 事件触发）
 *    SmartcarApp_TaskControl   控制 PID + 执行器下发（10ms 周期）
 *    DebugDisplayService_Update TFT 调试显示（100ms 周期）
 */
#include "smartcar_app.h"
#include "control.h"
#include "debug_display.h"
#include "feedback_service.h"
#include "scheduler.h"
#include "event.h"
#include "sensor.h"
#include "vision.h"

//******************************** Types ************************************//
typedef struct
{
    task_fn_t    handler;
    uint32_t     period_ms;
    event_mask_t trigger;
} smartcar_app_task_desc_t;
//******************************** Types ************************************//

//******************************** Variables ********************************//
static uint8_t s_task_register_fail_count = 0U;
//******************************** Variables ********************************//

/**
 * @brief 陀螺仪处理任务
 *        EVT_GYRO_10MS 触发，10ms 周期。
 */
static void SmartcarApp_TaskGyro(event_mask_t events)
{
    if ((events & EVT_GYRO_10MS) == 0U)
    {
        return;
    }

    SensorService_ProcessGyro10ms();
}

/**
 * @brief 编码器测速任务（原 ISR 中的 Encoder_CalculateSpeed 逻辑）
 *        EVT_ENCODER_50MS 触发，50ms 周期。
 */
static void SmartcarApp_TaskEncoder(event_mask_t events)
{
    if ((events & EVT_ENCODER_50MS) == 0U)
    {
        return;
    }

    SensorService_ProcessEncoder50ms();
}

/**
 * @brief 视觉处理任务
 *        EVT_CAM_FRAME 触发。处理一帧图像、检测特殊元素、蜂鸣器提示。
 */
static void SmartcarApp_TaskVision(event_mask_t events)
{
    uint8_t element = 0;

    if ((events & EVT_CAM_FRAME) == 0U)
    {
        return;
    }

    Vision_Process();
    DebugDisplayService_DrawVisionLines();

    element = Vision_DetectElement();
    FeedbackService_NotifyTrackElement(element);

    Vision_ClearFrameReady();
}

/**
 * @brief 控制任务（周期 10ms）
 *        基于视觉偏差与编码器速度计算 PID，并下发到执行器。
 */
static void SmartcarApp_TaskControl(event_mask_t events)
{
    (void)events;
    Control_Update();
    Actuator_Apply();
}

/**
 * @brief 注册一个应用任务。
 *
 * 处理步骤：
 *  1. 调用调度器注册任务。
 *  2. 若注册失败，记录失败次数供诊断读取。
 *
 * @param[in] p_task : 应用任务描述。
 *
 * @return void : 无返回值。
 *
 */
static void SmartcarApp_RegisterTask(const smartcar_app_task_desc_t *p_task)
{
    int8_t task_index = -1;

    if (p_task == 0)
    {
        s_task_register_fail_count++;
        return;
    }

    task_index = scheduler_add(p_task->handler, p_task->period_ms, p_task->trigger);
    if (task_index < 0)
    {
        s_task_register_fail_count++;
    }
}

/**
 * @brief 应用层初始化入口。
 *
 * 处理步骤：
 *  1. 清零任务注册诊断计数。
 *  2. 按优先级从高到低注册业务任务。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarApp_Init(void)
{
    uint8_t i = 0U;
    static const smartcar_app_task_desc_t s_app_tasks[] =
    {
        {SmartcarApp_TaskGyro,       0U,   EVT_GYRO_10MS},
        {SmartcarApp_TaskEncoder,    0U,   EVT_ENCODER_50MS},
        {SmartcarApp_TaskVision,     0U,   EVT_CAM_FRAME},
        {SmartcarApp_TaskControl,    10U,  EVT_NONE},
        {DebugDisplayService_Update, 100U, EVT_NONE}
    };

    s_task_register_fail_count = 0U;

    for (i = 0U; i < (uint8_t)(sizeof(s_app_tasks) / sizeof(s_app_tasks[0])); i++)
    {
        SmartcarApp_RegisterTask(&s_app_tasks[i]);
    }
}

/**
 * @brief 获取应用任务注册失败次数。
 *
 * @return uint8_t : 注册失败次数。
 *
 * */
uint8_t SmartcarApp_GetTaskRegisterFailCount(void)
{
    return s_task_register_fail_count;
}

/**
 * @brief 应用层单次循环
 *        调度器分发 → 蜂鸣器时序。
 */
void SmartcarApp_RunOnce(void)
{
    /* 调度器驱动全部任务 */
    scheduler_run();

    FeedbackService_Tick();
}
