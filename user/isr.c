/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          isr
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
/**
 * @file isr.c
 * @brief TC264 SDK 中断入口转发层。
 * @author GS_Mark
 *
 * @par 设计说明
 * IFX_INTERRUPT 宏约束入口必须保留在 user 层；具体硬件处理统一转发给 TC264 ISR 端口。
 */

#include "isr_config.h"
#include "isr.h"
#include "target/tc264/tc264_isr.h"

// TC264 中断入口受 IFX_INTERRUPT 宏约束保留在本文件，具体处理统一交给 Tc264Isr。
//----------------------------------------------------------------------
// **************************** PIT Interrupt Handlers ****************************

/**
 * @brief 编码器测速中断（CCU60 通道0，10ms 周期）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    Tc264Isr_Ccu60PitCh0();
}


/**
 * @brief 系统时间基 + 陀螺仪触发中断（CCU60 通道1，10ms 周期）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    Tc264Isr_Ccu60PitCh1();
}

// **************************** PIT Interrupt Handlers ****************************


// **************************** External Interrupt Handlers ****************************
// 摄像头 PCLK 默认占用通道 2 触发 DMA，因此这里不定义 ch2/ch6 CPU ISR。
/**
 * @brief 外部中断 ERU 通道3 与通道7 共用入口。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO)
{
    Tc264Isr_ExtiCh3Ch7();
}
// **************************** External Interrupt Handlers ****************************


// **************************** DMA Interrupt Handlers ****************************
/**
 * @brief 摄像头 DMA 采集完成中断（DMA 通道5）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO)
{
    Tc264Isr_DmaCh5();
}
// **************************** DMA Interrupt Handlers ****************************


// **************************** UART Interrupt Handlers ****************************
// 串口0默认作为调试串口
/**
 * @brief 串口0 接收中断（调试串口）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    Tc264Isr_Uart0Rx();
}


// 串口1默认连接到摄像头配置串口
/**
 * @brief 串口1 接收中断（摄像头配置串口）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
    Tc264Isr_Uart1Rx();
}

/**
 * @brief 串口3 接收中断（无线转串口模块）。
 *        入口层只转发到 ISR adapter。
 *
 * @return void。
 */
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
    Tc264Isr_Uart3Rx();
}

// 串口通讯错误中断
/**
 * @brief 串口0 通讯错误中断。
 *
 * @return void。
 */
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
    Tc264Isr_Uart0Error();
}
/**
 * @brief 串口1 通讯错误中断。
 *
 * @return void。
 */
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
    Tc264Isr_Uart1Error();
}
/**
 * @brief 串口3 通讯错误中断。
 *
 * @return void。
 */
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
    Tc264Isr_Uart3Error();
}
