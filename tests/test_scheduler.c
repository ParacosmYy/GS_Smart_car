#include <stdint.h>
#include <stdio.h>

#include "scheduler.h"

static uint32_t s_task_run_count = 0U;

event_mask_t event_get(void)
{
    return EVT_NONE;
}

static void TestTask(event_mask_t events)
{
    (void)events;
    s_task_run_count++;
}

static int ExpectEqualS32(const char *name, int32_t actual, int32_t expected)
{
    if (actual != expected)
    {
        (void)printf("%s: expected %ld, got %ld\n",
                     name,
                     (long)expected,
                     (long)actual);
        return 1;
    }

    return 0;
}

static int ExpectEqualU32(const char *name, uint32_t actual, uint32_t expected)
{
    if (actual != expected)
    {
        (void)printf("%s: expected %lu, got %lu\n",
                     name,
                     (unsigned long)expected,
                     (unsigned long)actual);
        return 1;
    }

    return 0;
}

int main(void)
{
    int failures = 0;
    uint8_t i = 0U;

    scheduler_init();
    failures += ExpectEqualS32("空任务拒绝", scheduler_add(0, 10U, EVT_NONE), -1);
    failures += ExpectEqualS32("首个任务槽位", scheduler_add(TestTask, 10U, EVT_NONE), 0);

    Scheduler_AddTickFromIsr(10U);
    scheduler_run();
    failures += ExpectEqualU32("周期任务执行", s_task_run_count, 1U);

    scheduler_init();
    for (i = 0U; i < SCH_MAX_TASKS; i++)
    {
        failures += ExpectEqualS32("任务表填充", scheduler_add(TestTask, 0U, EVT_CAM_FRAME), (int32_t)i);
    }

    failures += ExpectEqualS32("任务表满拒绝", scheduler_add(TestTask, 0U, EVT_CAM_FRAME), -1);

    return failures;
}
