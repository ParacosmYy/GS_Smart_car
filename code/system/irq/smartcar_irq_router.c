#include "smartcar_irq_router.h"

#include "event.h"
#include "scheduler.h"

void SmartcarIrq_Dispatch(smartcar_irq_source_t source)
{
    const target_irq_route_t *p_routes;
    uint16_t route_count = 0U;
    uint16_t i;

    p_routes = TargetPlatform_GetIrqRoutes(&route_count);
    if (p_routes == 0)
    {
        return;
    }

    for (i = 0U; i < route_count; i++)
    {
        const target_irq_route_t *p_route = &p_routes[i];
        irq_fact_t facts = IRQ_FACT_NONE;

        if (p_route->source == source)
        {
            if (p_route->handler != 0)
            {
                facts = p_route->handler();
            }

            if ((p_route->fact_mask != IRQ_FACT_NONE) && ((facts & p_route->fact_mask) == 0U))
            {
                return;
            }

            if (p_route->tick_ms != 0U)
            {
                Scheduler_AddTickFromIsr(p_route->tick_ms);
            }

            if (p_route->event != EVT_NONE)
            {
                event_post_from_isr((event_mask_t)p_route->event);
            }

            return;
        }
    }
}
