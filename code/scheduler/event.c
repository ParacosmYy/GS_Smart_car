/**
 * @file event.c
 * @brief Event flag system implementation.
 */
#include "event.h"
#include "platform.h"

static volatile event_mask_t s_events = 0;
static volatile uint32_t s_gyro_10ms_pending = 0;

void event_post_from_isr(event_mask_t events)
{
    event_mask_t normal_events = EVT_NONE;
    uint32_t irq_state = 0;

    irq_state = pal_irq_global_disable();
    normal_events = events & (~EVT_GYRO_10MS);
    s_events |= normal_events;

    if ((events & EVT_GYRO_10MS) != 0U)
    {
        if (s_gyro_10ms_pending < UINT32_MAX)
        {
            s_gyro_10ms_pending++;
        }
    }
    pal_irq_global_restore(irq_state);
}

void event_set_isr(event_mask_t events)
{
    event_post_from_isr(events);
}

event_mask_t event_get(void)
{
    event_mask_t pending = EVT_NONE;
    uint32_t irq_state = 0;

    irq_state = pal_irq_global_disable();
    pending = s_events;
    s_events = 0;

    if (s_gyro_10ms_pending > 0U)
    {
        pending |= EVT_GYRO_10MS;
        s_gyro_10ms_pending--;
    }
    pal_irq_global_restore(irq_state);

    return pending;
}
