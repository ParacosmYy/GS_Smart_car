#include "isr_adapter.h"
#include "system/irq/smartcar_irq_router.h"

static const smartcar_irq_target_route_t s_tc264_irq_routes[] =
{
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0},
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1},
    {SMARTCAR_IRQ_SOURCE_EXTI_CH3_CH7,  IsrAdapter_ExtiCh3Ch7},
    {SMARTCAR_IRQ_SOURCE_DMA_CH5,       IsrAdapter_DmaCh5},
    {SMARTCAR_IRQ_SOURCE_UART0_RX,      IsrAdapter_Uart0Rx},
    {SMARTCAR_IRQ_SOURCE_UART1_RX,      IsrAdapter_Uart1Rx},
    {SMARTCAR_IRQ_SOURCE_UART3_RX,      IsrAdapter_Uart3Rx},
    {SMARTCAR_IRQ_SOURCE_UART0_ERROR,   IsrAdapter_Uart0Error},
    {SMARTCAR_IRQ_SOURCE_UART1_ERROR,   IsrAdapter_Uart1Error},
    {SMARTCAR_IRQ_SOURCE_UART3_ERROR,   IsrAdapter_Uart3Error},
};

const smartcar_irq_target_route_t *TargetPlatform_GetIrqRoutes(uint16_t *p_count)
{
    if (p_count != 0)
    {
        *p_count = (uint16_t)(sizeof(s_tc264_irq_routes) / sizeof(s_tc264_irq_routes[0]));
    }

    return s_tc264_irq_routes;
}
