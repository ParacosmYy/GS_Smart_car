#include <stdint.h>
#include <stdio.h>
#include "event.h"

static uint32_t s_disable_count = 0;
static uint32_t s_restore_count = 0;
static uint32_t s_last_restore_state = 0;

uint32_t pal_irq_global_disable(void)
{
    s_disable_count++;
    return 7U;
}

void pal_irq_global_restore(uint32_t state)
{
    s_restore_count++;
    s_last_restore_state = state;
}

static int expect_equal_u32(const char *name, uint32_t actual, uint32_t expected)
{
    if (actual != expected)
    {
        (void)printf("%s: expected %u, got %u\n", name, expected, actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failures = 0;

    event_post_from_isr(EVT_CAM_FRAME);
    event_post_from_isr(EVT_GYRO_10MS);

    failures += expect_equal_u32("待处理事件",
                                 event_get(),
                                 EVT_CAM_FRAME | EVT_GYRO_10MS);
    failures += expect_equal_u32("事件已清空", event_get(), EVT_NONE);

    event_post_from_isr(EVT_GYRO_10MS);
    event_post_from_isr(EVT_GYRO_10MS);
    event_post_from_isr(EVT_GYRO_10MS);

    failures += expect_equal_u32("陀螺仪节拍1", event_get(), EVT_GYRO_10MS);
    failures += expect_equal_u32("陀螺仪节拍2", event_get(), EVT_GYRO_10MS);
    failures += expect_equal_u32("陀螺仪节拍3", event_get(), EVT_GYRO_10MS);
    failures += expect_equal_u32("陀螺仪节拍已取完", event_get(), EVT_NONE);

    failures += expect_equal_u32("disable count", s_disable_count, 11U);
    failures += expect_equal_u32("restore count", s_restore_count, 11U);
    failures += expect_equal_u32("restore state", s_last_restore_state, 7U);

    return failures;
}
