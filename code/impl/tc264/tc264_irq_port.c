/**
 * @file tc264_irq_port.c
 * @brief TC264 中断端口实现。
 *
 * 本文件是 SDK 中断入口与 System IRQ router 之间的目标适配端口。
 * 通过 irq_port.h 暴露硬件路由表，通过 irq_port_dispatch()
 * 将中断事实路由到 System 层 —— Impl 不直接依赖 System 头文件。
 */

#include "tc264_irq_port.h"

#include "config.h"
#include "isr_adapter.h"
#include "platform/system/irq_port.h"

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
/**
 * @brief TC264 端口路由表（Platform contract 格式）。
 *
 * 只包含硬件层信息：source / handler / fact_mask。
 * 调度事件和时间基的映射由 System 层 IrqPortAdapter 在启动时完成。
 */
static const irq_port_route_t s_tc264_port_routes[] =
{
    {TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH0, IsrAdapter_Ccu60PitCh0,
     IRQ_FACT_ENCODER_WINDOW},
    {TC264_IRQ_PORT_SOURCE_CCU60_PIT_CH1, IsrAdapter_Ccu60PitCh1,
     IRQ_FACT_GYRO_TICK},
    {TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH0, IsrAdapter_Ccu61PitCh0,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_CCU61_PIT_CH1, IsrAdapter_Ccu61PitCh1,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH0_CH4, IsrAdapter_ExtiCh0Ch4,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH1_CH5, IsrAdapter_ExtiCh1Ch5,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_EXTI_CH3_CH7, IsrAdapter_ExtiCh3Ch7,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_DMA_CH5, IsrAdapter_DmaCh5,
     IRQ_FACT_CAMERA_FRAME},
    {TC264_IRQ_PORT_SOURCE_UART0_TX, IsrAdapter_Uart0Tx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART0_RX, IsrAdapter_Uart0Rx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART1_TX, IsrAdapter_Uart1Tx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART1_RX, IsrAdapter_Uart1Rx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART2_TX, IsrAdapter_Uart2Tx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART2_RX, IsrAdapter_Uart2Rx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART3_TX, IsrAdapter_Uart3Tx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART3_RX, IsrAdapter_Uart3Rx,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART0_ERROR, IsrAdapter_Uart0Error,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART1_ERROR, IsrAdapter_Uart1Error,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART2_ERROR, IsrAdapter_Uart2Error,
     IRQ_FACT_NONE},
    {TC264_IRQ_PORT_SOURCE_UART3_ERROR, IsrAdapter_Uart3Error,
     IRQ_FACT_NONE}
};
//******************************** Variables ********************************//

//******************************** Implement ********************************//

/**
 * @brief 获取 TC264 端口路由表（Platform contract 实现）。
 *
 * 由 System 层 IrqPortAdapter_Init() 在启动时调用。
 *
 * @param[out] p_count 路由表项个数。
 * @return const irq_port_route_t* 路由表指针。
 */
const irq_port_route_t *irq_port_get_routes(uint16_t *p_count)
{
    if (p_count != 0)
    {
        *p_count = (uint16_t)(sizeof(s_tc264_port_routes)
                              / sizeof(s_tc264_port_routes[0]));
    }
    return s_tc264_port_routes;
}

/**
 * @brief 分发 TC264 目标中断源。
 *
 * 通过 irq_port_dispatch() 将 source 传递给 System IRQ router。
 * 避免了直接依赖 system/irq/smartcar_irq_router.h 或暴露全局变量。
 *
 * @param[in] source : TC264 端口内部中断源。
 */
static void Tc264IrqPort_Dispatch(tc264_irq_port_source_t source)
{
    irq_port_dispatch((irq_source_t)source);
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
