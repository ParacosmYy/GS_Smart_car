/**
 * @file tc264_irq_port.c
 * @brief TC264 中断端口实现。
 *
 * 本文件是 SDK 中断入口与 system IRQ router 之间的目标适配端口。
 * TC264 source 编号和 adapter 绑定只在本文件内部可见，避免 SDK entry
 * 或 System Runtime 直接依赖目标芯片枚举。
 */

#include "tc264_irq_port.h"

#include "config.h"
#include "event.h"
#include "isr_adapter.h"
#include "system/irq/smartcar_irq_router.h"

//******************************** Types ************************************//
typedef enum
{
    TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH0 = 0,
    TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH1,
    TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH0,
    TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH1,
    TC264_IRQ_PORT_SOURCE_EXTI_CH0_CH4,
    TC264_IRQ_PORT_SOURCE_EXTI_CH1_CH5,
    TC264_IRQ_PORT_SOURCE_EXTI_CH3_CH7,
    TC264_IRQ_PORT_SOURCE_DMA_CH5,
    TC264_IRQ_PORT_SOURCE_UART0_TX,
    TC264_IRQ_PORT_SOURCE_UART0_RX,
    TC264_IRQ_PORT_SOURCE_UART1_TX,
    TC264_IRQ_PORT_SOURCE_UART1_RX,
    TC264_IRQ_PORT_SOURCE_UART2_TX,
    TC264_IRQ_PORT_SOURCE_UART2_RX,
    TC264_IRQ_PORT_SOURCE_UART3_TX,
    TC264_IRQ_PORT_SOURCE_UART3_RX,
    TC264_IRQ_PORT_SOURCE_UART0_ERROR,
    TC264_IRQ_PORT_SOURCE_UART1_ERROR,
    TC264_IRQ_PORT_SOURCE_UART2_ERROR,
    TC264_IRQ_PORT_SOURCE_UART3_ERROR,
    TC264_IRQ_PORT_SOURCE_MAX
} tc264_irq_port_source_t;
//******************************** Types ************************************//

//******************************** Declaring ********************************//
static void Tc264IrqPort_Dispatch(tc264_irq_port_source_t source);
//******************************** Declaring ********************************//

//******************************** Variables ********************************//
static const smartcar_irq_route_t s_tc264_irq_routes[] =
{
    {TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0,
     SMARTCAR_IRQ_FACT_ENCODER_WINDOW, EVT_ENCODER_50MS, 0U},
    {TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1,
     SMARTCAR_IRQ_FACT_GYRO_TICK, EVT_GYRO_10MS, PIT_PERIOD_MS},
    {TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH0, IsrAdapter_Ccu61PitCh0,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH1, IsrAdapter_Ccu61PitCh1,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH0_CH4, IsrAdapter_ExtiCh0Ch4,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH1_CH5, IsrAdapter_ExtiCh1Ch5,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH3_CH7, IsrAdapter_ExtiCh3Ch7,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_DMA_CH5, IsrAdapter_DmaCh5,
     SMARTCAR_IRQ_FACT_CAMERA_FRAME, EVT_CAM_FRAME, 0U},
    {TC264_IRQ_PORT_SOURCE_UART0_TX, IsrAdapter_Uart0Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART0_RX, IsrAdapter_Uart0Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART1_TX, IsrAdapter_Uart1Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART1_RX, IsrAdapter_Uart1Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART2_TX, IsrAdapter_Uart2Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART2_RX, IsrAdapter_Uart2Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART3_TX, IsrAdapter_Uart3Tx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART3_RX, IsrAdapter_Uart3Rx,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART0_ERROR, IsrAdapter_Uart0Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART1_ERROR, IsrAdapter_Uart1Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART2_ERROR, IsrAdapter_Uart2Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U},
    {TC264_IRQ_PORT_SOURCE_UART3_ERROR, IsrAdapter_Uart3Error,
     SMARTCAR_IRQ_FACT_NONE, EVT_NONE, 0U}
};
//******************************** Variables ********************************//

//******************************** Implement ********************************//
/**
 * @brief 初始化 TC264 中断端口路由。
 *
 * 处理步骤：
 *  1. 计算 TC264 静态路由表项数量。
 *  2. 将路由表注册到 system IRQ router。
 *
 * @return void : 无返回值。
 *
 * */
void Tc264IrqPort_Init(void)
{
    SmartcarIrqRouter_Init(s_tc264_irq_routes,
                           (uint16_t)(sizeof(s_tc264_irq_routes) / sizeof(s_tc264_irq_routes[0])));
}

/**
 * @brief 分发 TC264 目标中断源。
 *
 * 处理步骤：
 *  1. 将目标端口内部 source 转换为通用 router source。
 *  2. 委托 system IRQ router 完成 fact/event 发布。
 *
 * @param[in] source : TC264 端口内部中断源。
 *
 * @return void : 无返回值。
 *
 * */
static void Tc264IrqPort_Dispatch(tc264_irq_port_source_t source)
{
    SmartcarIrqRouter_Dispatch((smartcar_irq_source_t)source);
}

void Tc264IrqPort_OnCcu60PitCh0(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH0);
}

void Tc264IrqPort_OnCcu60PitCh1(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH1);
}

void Tc264IrqPort_OnCcu61PitCh0(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH0);
}

void Tc264IrqPort_OnCcu61PitCh1(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH1);
}

void Tc264IrqPort_OnExtiCh0Ch4(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_EXTI_CH0_CH4);
}

void Tc264IrqPort_OnExtiCh1Ch5(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_EXTI_CH1_CH5);
}

void Tc264IrqPort_OnExtiCh3Ch7(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_EXTI_CH3_CH7);
}

void Tc264IrqPort_OnDmaCh5(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_DMA_CH5);
}

void Tc264IrqPort_OnUart0Tx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART0_TX);
}

void Tc264IrqPort_OnUart0Rx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART0_RX);
}

void Tc264IrqPort_OnUart1Tx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART1_TX);
}

void Tc264IrqPort_OnUart1Rx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART1_RX);
}

void Tc264IrqPort_OnUart2Tx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART2_TX);
}

void Tc264IrqPort_OnUart2Rx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART2_RX);
}

void Tc264IrqPort_OnUart3Tx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART3_TX);
}

void Tc264IrqPort_OnUart3Rx(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART3_RX);
}

void Tc264IrqPort_OnUart0Error(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART0_ERROR);
}

void Tc264IrqPort_OnUart1Error(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART1_ERROR);
}

void Tc264IrqPort_OnUart2Error(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART2_ERROR);
}

void Tc264IrqPort_OnUart3Error(void)
{
    Tc264IrqPort_Dispatch(TC264_IRQ_PORT_SOURCE_UART3_ERROR);
}
//******************************** Implement ********************************//
