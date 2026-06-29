#include "platform/target/target_platform.h"

#include "config.h"
#include "event.h"
#include "isr_adapter.h"

static const target_irq_route_t s_tc264_irq_routes[] =
{
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0, IRQ_FACT_ENCODER_WINDOW, EVT_ENCODER_50MS, 0U},
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1, IRQ_FACT_GYRO_TICK, EVT_GYRO_10MS, PIT_PERIOD_MS},
    {SMARTCAR_IRQ_SOURCE_EXTI_CH3_CH7,  IsrAdapter_ExtiCh3Ch7,  IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_DMA_CH5,       IsrAdapter_DmaCh5,      IRQ_FACT_CAMERA_FRAME, EVT_CAM_FRAME, 0U},
    {SMARTCAR_IRQ_SOURCE_UART0_RX,      IsrAdapter_Uart0Rx,     IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART1_RX,      IsrAdapter_Uart1Rx,     IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART3_RX,      IsrAdapter_Uart3Rx,     IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART0_ERROR,   IsrAdapter_Uart0Error,  IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART1_ERROR,   IsrAdapter_Uart1Error,  IRQ_FACT_NONE, EVT_NONE, 0U},
    {SMARTCAR_IRQ_SOURCE_UART3_ERROR,   IsrAdapter_Uart3Error,  IRQ_FACT_NONE, EVT_NONE, 0U},
};

const target_irq_route_t *TargetPlatform_GetIrqRoutes(uint16_t *p_count)
{
    if (p_count != 0)
    {
        *p_count = (uint16_t)(sizeof(s_tc264_irq_routes) / sizeof(s_tc264_irq_routes[0]));
    }

    return s_tc264_irq_routes;
}
