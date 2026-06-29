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
 * @brief 初始化协作调度器。
 *
 * Steps:
 *   1. 清零任务数量和系统毫秒计数。
 *   2. 标记任务表全部 inactive。
 *
 * @return void。
 */
void scheduler_init(void)
{
    s_count = 0;
    g_system_ms = 0;

    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++)
    {
        s_tasks[i].active = 0;
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
 * @brief 注册一个协作任务。
 *
 * Steps:
 *   1. 校验任务函数和任务表容量。
 *   2. 写入周期、事件触发掩码和初始状态。
 *   3. 返回任务索引。
 *
 * @param[in] fn 任务函数。
 * @param[in] period_ms 周期任务间隔；0 表示非周期。
 * @param[in] trigger 事件触发掩码；EVT_NONE 表示非事件触发。
 * @return 任务索引；失败返回 -1。
 */
int8_t scheduler_add(task_fn_t fn, uint32_t period_ms, event_mask_t trigger)
{
    if (fn == 0)
    {
        return -1;
    }

    if (s_count >= SCH_MAX_TASKS)
    {
        return -1;
    }

    sch_task_t *t      = &s_tasks[s_count];
    t->fn              = fn;
    t->period_ms       = period_ms;
    t->last_run        = 0;
    t->trigger         = trigger;
    t->active          = 1;

    return (int8_t)s_count++;
}

/**
 * @brief 运行一次协作调度。
 *
 * Steps:
 *   1. 获取并消费本轮事件。
 *   2. 遍历已注册任务。
 *   3. 命中事件触发或周期触发时调用任务函数。
 *
 * @return void。
 */
void scheduler_run(void)
{
    event_mask_t events = event_get();
    uint32_t now = Scheduler_GetNowMs();

    uint8_t i;
    for (i = 0; i < s_count; i++)
    {
        sch_task_t *t = &s_tasks[i];

        if (!t->active)
        {
            continue;
        }

        uint8_t should_run = 0;

        /* 事件触发 */
        if (t->trigger != 0 && (events & t->trigger) != 0)
        {
            should_run = 1;
        }

        /* 周期触发 */
        if (t->period_ms != 0 && (now - t->last_run) >= t->period_ms)
        {
            should_run     = 1;
            t->last_run    = now;
        }

        if (should_run)
        {
            t->fn(events);
        }
    }
}
