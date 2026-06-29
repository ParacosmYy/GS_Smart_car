#include "smartcar_irq_router.h"

#include "config.h"
#include "event.h"
#include "scheduler.h"

static void SmartcarIrq_PostFacts(irq_fact_t facts)
{
    if ((facts & IRQ_FACT_ENCODER_WINDOW) != 0U)
    {
        event_post_from_isr(EVT_ENCODER_50MS);
    }

    if ((facts & IRQ_FACT_GYRO_TICK) != 0U)
    {
        Scheduler_AddTickFromIsr(PIT_PERIOD_MS);
        event_post_from_isr(EVT_GYRO_10MS);
    }

    if ((facts & IRQ_FACT_CAMERA_FRAME) != 0U)
    {
        event_post_from_isr(EVT_CAM_FRAME);
    }
}

void SmartcarIrq_Dispatch(smartcar_irq_source_t source)
{
    const smartcar_irq_target_route_t *p_routes;
    uint16_t route_count = 0U;
    uint16_t i;

    p_routes = TargetPlatform_GetIrqRoutes(&route_count);
    if (p_routes == 0)
    {
        return;
    }

    for (i = 0U; i < route_count; i++)
    {
        const smartcar_irq_target_route_t *p_route = &p_routes[i];
        irq_fact_t facts = IRQ_FACT_NONE;

        if (p_route->source == source)
        {
            if (p_route->handler != 0)
            {
                facts = p_route->handler();
            }

            SmartcarIrq_PostFacts(facts);
            return;
        }
    }
}
