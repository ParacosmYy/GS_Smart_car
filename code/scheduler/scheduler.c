/**
 * @file scheduler.c
 * @brief Lightweight cooperative task scheduler implementation.
 */
#include "scheduler.h"

/* 系统毫秒计数器（由 PIT ISR 通过调度器 API 递增）*/
static volatile uint32_t g_system_ms = 0;

/* 任务表 */
static sch_task_t s_tasks[SCH_MAX_TASKS];
static uint8_t    s_count = 0;

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

void Scheduler_AddTickFromIsr(uint32_t elapsed_ms)
{
    g_system_ms += elapsed_ms;
}

uint32_t Scheduler_GetNowMs(void)
{
    return g_system_ms;
}

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
