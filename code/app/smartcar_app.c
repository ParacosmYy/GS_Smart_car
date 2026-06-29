/*
 * smartcar_app.c
 * @brief Smart car application scheduler implementation.
 *
 * Processing flow:
 * ISR entries do minimum bounded work and task processing is dispatched by
 * the cooperative scheduler:
 *    SensorTask_Gyro10ms       陀螺仪传感器服务处理（事件触发）
 *    SensorTask_Encoder50ms    编码器传感器服务处理（事件触发）
 *    VisionTask_OnFrame        视觉处理流水线（DMA 事件触发）
 *    ControlTask_10ms          控制输出（10ms 周期）
 *    FeedbackTask_Tick         非阻塞反馈时序（随帧推进）
 *    DiagnosticsTask_100ms     TFT 调试显示（100ms 周期）
 */
#include "smartcar_app.h"
#include "scheduler.h"
#include "event.h"
#include "service/smartcar_tasks.h"

//******************************** Types ************************************//
typedef struct
{
    task_fn_t    handler;
    uint32_t     period_ms;
    event_mask_t trigger;
} smartcar_app_task_desc_t;
//******************************** Types ************************************//

static void SmartcarApp_RegisterTask(const smartcar_app_task_desc_t *p_task);

//******************************** Variables ********************************//
static uint8_t s_task_register_fail_count = 0U;

static const smartcar_app_task_desc_t s_app_tasks[] =
{
    /* 事件任务优先注册，周期任务随后注册。 */
    { .handler = SensorTask_Gyro10ms,     .period_ms = 0U,   .trigger = EVT_GYRO_10MS },
    { .handler = SensorTask_Encoder50ms,  .period_ms = 0U,   .trigger = EVT_ENCODER_50MS },
    { .handler = VisionTask_OnFrame,      .period_ms = 0U,   .trigger = EVT_CAM_FRAME },
    { .handler = ControlTask_10ms,        .period_ms = 10U,  .trigger = EVT_NONE },
    { .handler = FeedbackTask_Tick,       .period_ms = 0U,   .trigger = EVT_CAM_FRAME },
    { .handler = DiagnosticsTask_100ms,   .period_ms = 100U, .trigger = EVT_NONE }
};

static const uint8_t s_app_task_count = (uint8_t)(sizeof(s_app_tasks) / sizeof(s_app_tasks[0]));
//******************************** Variables ********************************//

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

    s_task_register_fail_count = 0U;

    for (i = 0U; i < s_app_task_count; i++)
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
    scheduler_run();
}
