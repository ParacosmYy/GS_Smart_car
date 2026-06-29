/**
 * @file scheduler.c
 * @brief Lightweight cooperative task scheduler implementation.
 * @author GS_Mark
 *
 * @par 设计说明
 * 调度器支持事件触发任务和周期任务。ISR 只更新时间和事件，实际业务函数均在主循环运行。
 */
#include "scheduler.h"

/* 系统毫秒计数器（由 PIT ISR 通过调度器 API 递增）*/
static volatile uint32_t g_system_ms = 0;

/* 任务表 */
static sch_task_t s_tasks[SCH_MAX_TASKS];
static uint8_t    s_count = 0;

/**
 * @brief 检查调度阶段是否有效。
 *
 * @param[in] phase 调度阶段。
 * @return 1 表示有效；0 表示非法。
 */
static uint8_t Scheduler_IsValidPhase(scheduler_task_phase_t phase)
{
    uint8_t is_valid = 0U;

    if ((phase >= SCHEDULER_TASK_PHASE_SENSOR_EVENT) &&
        (phase < SCHEDULER_TASK_PHASE_COUNT))
    {
        is_valid = 1U;
    }

    return is_valid;
}

/**
 * @brief 检查任务事件触发条件是否命中。
 *
 * @param[in] p_task 任务描述符。
 * @param[in] events 本轮事件快照。
 * @return 1 表示事件命中；0 表示未命中。
 */
static uint8_t Scheduler_IsEventReady(const sch_task_t *p_task,
                                      event_mask_t events)
{
    uint8_t is_ready = 0U;

    if ((p_task->trigger != EVT_NONE) &&
        ((events & p_task->trigger) != EVT_NONE))
    {
        is_ready = 1U;
    }

    return is_ready;
}

/**
 * @brief 尝试执行已进入当前阶段的任务。
 *
 * Steps:
 *   1. 检查事件触发条件。
 *   2. 检查周期触发条件，周期到期时刷新 last_run。
 *   3. 若事件或周期任一命中，则只调用一次任务函数。
 *
 * @param[in,out] p_task 任务描述符。
 * @param[in] events 本轮事件快照。
 * @param[in] now 当前调度时间。
 * @return 1 表示任务已执行；0 表示未执行。
 */
static uint8_t Scheduler_RunTaskIfReady(sch_task_t *p_task,
                                        event_mask_t events,
                                        uint32_t now)
{
    uint8_t event_ready = 0U;
    uint8_t period_ready = 0U;
    uint8_t did_run = 0U;

    event_ready = Scheduler_IsEventReady(p_task, events);
    if ((p_task->period_ms != 0U) &&
        ((now - p_task->last_run) >= p_task->period_ms))
    {
        period_ready = 1U;
        p_task->last_run = now;
    }

    if ((event_ready != 0U) || (period_ready != 0U))
    {
        p_task->fn(events);
        did_run = 1U;
    }
    else
    {
        did_run = 0U;
    }

    return did_run;
}

/**
 * @brief 初始化协作调度器。
 *
 * Steps:
 *   1. 清零任务数量和系统毫秒计数。
 *   2. 标记任务表全部 inactive。
 *
 * @return void。
 */
void Scheduler_Init(void)
{
    uint8_t i = 0U;

    s_count = 0U;
    g_system_ms = 0U;

    for (i = 0U; i < SCH_MAX_TASKS; i++)
    {
        s_tasks[i].fn = 0;
        s_tasks[i].period_ms = 0U;
        s_tasks[i].last_run = 0U;
        s_tasks[i].trigger = EVT_NONE;
        s_tasks[i].phase = SCHEDULER_TASK_PHASE_NORMAL_EVENT;
        s_tasks[i].active = 0U;
    }
}

/**
 * @brief 从 ISR 上下文累加调度器时间。
 *
 * @param[in] elapsed_ms 距上次 tick 的毫秒数。
 * @return void。
 */
void Scheduler_AddTickFromIsr(uint32_t elapsed_ms)
{
    g_system_ms += elapsed_ms;
}

/**
 * @brief 获取当前调度器毫秒计数。
 *
 * @return 当前系统毫秒计数。
 */
uint32_t Scheduler_GetNowMs(void)
{
    return g_system_ms;
}

/**
 * @brief 注册一个指定调度阶段的协作任务。
 *
 * Steps:
 *   1. 校验任务函数、任务表容量和调度阶段。
 *   2. 写入周期、事件触发掩码、显式阶段和初始状态。
 *   3. 返回任务索引。
 *
 * @param[in] fn 任务函数。
 * @param[in] period_ms 周期任务间隔；0 表示非周期。
 * @param[in] trigger 事件触发掩码；EVT_NONE 表示非事件触发。
 * @param[in] phase 显式调度阶段。
 * @return 任务索引；失败返回 -1。
 */
int8_t Scheduler_AddEx(task_fn_t fn,
                       uint32_t period_ms,
                       event_mask_t trigger,
                       scheduler_task_phase_t phase)
{
    sch_task_t *p_task = 0;
    int8_t task_index = -1;

    if (fn == 0)
    {
        return -1;
    }

    if (s_count >= SCH_MAX_TASKS)
    {
        return -1;
    }

    if (Scheduler_IsValidPhase(phase) == 0U)
    {
        return -1;
    }

    p_task = &s_tasks[s_count];
    p_task->fn = fn;
    p_task->period_ms = period_ms;
    p_task->last_run = 0U;
    p_task->trigger = trigger;
    p_task->phase = phase;
    p_task->active = 1U;

    task_index = (int8_t)s_count;
    s_count++;

    return task_index;
}

/**
 * @brief 运行一次协作调度。
 *
 * Steps:
 *   1. 获取并消费本轮事件。
 *   2. 按 scheduler_task_phase_t 枚举顺序遍历显式阶段。
 *   3. 每个阶段内按注册顺序执行到期或事件命中的任务。
 *
 * @return void。
 */
void Scheduler_Run(void)
{
    event_mask_t events = Event_Get();
    uint32_t now = Scheduler_GetNowMs();
    uint8_t phase = 0U;
    uint8_t i = 0U;

    for (phase = 0U; phase < (uint8_t)SCHEDULER_TASK_PHASE_COUNT; phase++)
    {
        for (i = 0U; i < s_count; i++)
        {
            sch_task_t *p_task = &s_tasks[i];

            if (p_task->active == 0U)
            {
                continue;
            }

            if (p_task->phase != (scheduler_task_phase_t)phase)
            {
                continue;
            }

            (void)Scheduler_RunTaskIfReady(p_task, events, now);
        }
    }
}
