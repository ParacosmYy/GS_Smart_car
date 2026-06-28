/**
 * @file event.c
 * @brief 事件标志系统实现
 */
#include "event.h"

static volatile event_mask_t s_events = 0;

void event_set_isr(event_mask_t events)
{
    s_events |= events;
}

event_mask_t event_get(void)
{
    event_mask_t pending = s_events;
    s_events = 0;
    return pending;
}
