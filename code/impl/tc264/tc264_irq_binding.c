/**
 * @file tc264_irq_binding.c
 * @brief TC264 interrupt source route binding.
 *
 * This file is the target-specific route table. Porting to another MCU should
 * provide a new binding file and keep system/irq unchanged.
 */

#include "tc264_irq_binding.h"

#include "config.h"
#include "event.h"
#include "isr_adapter.h"

//******************************** Variables ********************************//
static const smartcar_irq_route_t s_tc264_irq_routes[] =
{
    {TC264_IRQ_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0,
     SMARTCAR_IRQ_FACT_ENCODER_WINDOW, EVT_ENCODER_50MS, 0U},
    {TC264_IRQ_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1,
     SMARTCAR_IRQ_FACT_GYRO_TICK, EVT_GYRO_10MS, PIT_PERIOD_MS},
    {TC264_IRQ_SOURCE_CCU61_PIT_CH0, IsrAdapter_Ccu61PitCh0,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_CCU61_PIT_CH1, IsrAdapter_Ccu61PitCh1,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_EXTI_CH0_CH4, IsrAdapter_ExtiCh0Ch4,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_EXTI_CH1_CH5, IsrAdapter_ExtiCh1Ch5,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_EXTI_CH3_CH7, IsrAdapter_ExtiCh3Ch7,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_DMA_CH5, IsrAdapter_DmaCh5,
     SMARTCAR_IRQ_FACT_CAMERA_FRAME, EVT_CAM_FRAME, 0U},
    {TC264_IRQ_SOURCE_UART0_TX, IsrAdapter_Uart0Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART0_RX, IsrAdapter_Uart0Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART1_TX, IsrAdapter_Uart1Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART1_RX, IsrAdapter_Uart1Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART2_TX, IsrAdapter_Uart2Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART2_RX, IsrAdapter_Uart2Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART3_TX, IsrAdapter_Uart3Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART3_RX, IsrAdapter_Uart3Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART0_ERROR, IsrAdapter_Uart0Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART1_ERROR, IsrAdapter_Uart1Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART2_ERROR, IsrAdapter_Uart2Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_SOURCE_UART3_ERROR, IsrAdapter_Uart3Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U}
};
//******************************** Variables ********************************//

//******************************** Implement ********************************//
/**
 * @brief 注册 TC264 中断路由表。
 *
 * 处理步骤：
 *  1. 计算 TC264 静态路由表项数量。
 *  2. 将路由表注册到 system IRQ router。
 *
 * @return void : 无返回值。
 *
 * */
void Tc264IrqBinding_Init(void)
{
    SmartcarIrqRouter_Init(s_tc264_irq_routes,
                           (uint16_t)(sizeof(s_tc264_irq_routes) / sizeof(s_tc264_irq_routes[0])));
}
//******************************** Implement ********************************//
