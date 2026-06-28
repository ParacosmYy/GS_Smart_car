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

#include "isr_config.h"
#include "isr.h"
#include "platform.h"
#include "event.h"
#include "scheduler.h"   /* g_system_ms */

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 pal_irq_global_ctrl(0); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 interrupt_global_disable(); 来拒绝响应任何的中断，因此需要我们自己手动调用 pal_irq_global_ctrl(0); 来开启中断的响应。
//----------------------------------------------------------------------
/* 编码器测速累加器：ISR 每 10ms 累加原始计数，累计 MAX_SAMPLES 次后置
   EVT_ENCODER_50MS 事件，由主循环 task_encoder 折算平均速度并清零。*/
int  left_speed_sum  = 0;
int  right_speed_sum = 0;
int  sample_count    = 0;
const int MAX_SAMPLES = 5;   /* 采样窗口（3-5 次），50ms 折算周期 */

// **************************** PIT中断函数 ****************************

/**
 * @brief 编码器测速中断（CCU60 通道0，10ms 周期）。
 *        每 tick 累加左右编码器计数，累计 MAX_SAMPLES（5）次后置
 *        EVT_ENCODER_50MS 事件，由主循环 task_encoder 折算平均速度。
 */
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_pit_clear_flag(PAL_CH_PIT_0);

    /* Accumulate raw encoder counts for this 10 ms tick. */
    left_speed_sum += pal_encoder_get(PAL_CH_ENCODER_L);
    right_speed_sum += pal_encoder_get(PAL_CH_ENCODER_R);
    sample_count++;

    /* Every MAX_SAMPLES ticks, signal main loop to compute averaged speed. */
    if (sample_count >= MAX_SAMPLES)
    {
        event_set_isr(EVT_ENCODER_50MS);
    }
}


/**
 * @brief 系统时间基 + 陀螺仪触发中断（CCU60 通道1，10ms 周期）。
 *        递增 g_system_ms（调度器周期判断依赖），并置 EVT_GYRO_10MS
 *        事件通知主循环执行陀螺仪采样、零漂补偿与角度积分。
 */
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_pit_clear_flag(PAL_CH_PIT_1);

    g_system_ms  += 10;                         // 系统时间基（调度器周期判断依赖）
    pit_ch1_count++;

    event_set_isr(EVT_GYRO_10MS);               // 通知主循环执行陀螺仪处理
}

/**
 * @brief CCU61 通道0 周期中断（当前空置，预留给扩展任务）。
 */
IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_pit_clear_flag(PAL_CH_PIT_2);




}

/**
 * @brief CCU61 通道1 周期中断（当前空置，预留给扩展任务）。
 */
IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_pit_clear_flag(PAL_CH_PIT_3);




}
// **************************** PIT中断函数 ****************************


// **************************** 外部中断函数 ****************************
/**
 * @brief 外部中断 ERU 通道0 与通道4 共用入口（当前空置）。
 *        通道0 对应 P15_4，通道4 对应 P15_5，在中断内通过标志位区分来源。
 */
IFX_INTERRUPT(exti_ch0_ch4_isr, 0, EXTI_CH0_CH4_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    if(exti_flag_get(ERU_CH0_REQ0_P15_4))           // 通道0中断
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);

    }

    if(exti_flag_get(ERU_CH4_REQ13_P15_5))          // 通道4中断
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);




    }
}

/**
 * @brief 外部中断 ERU 通道1 与通道5 共用入口（ToF 已移除，当前空置）。
 *        通道1 对应 P14_3，通道5 对应 P15_8。
 */
IFX_INTERRUPT(exti_ch1_ch5_isr, 0, EXTI_CH1_CH5_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套

    if(exti_flag_get(ERU_CH1_REQ10_P14_3))          // 通道1中断
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);

        // ToF handler removed - no ToF sensor configured
        // tof_module_exti_handler();                  // ToF 模块 INT 更新中断

    }

    if(exti_flag_get(ERU_CH5_REQ1_P15_8))           // 通道5中断
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);


    }
}

// 由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
// IFX_INTERRUPT(exti_ch2_ch6_isr, 0, EXTI_CH2_CH6_INT_PRIO)
// {
//  pal_irq_global_ctrl(0);                     // 开启中断嵌套
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // 通道2中断
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // 通道6中断
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }
/**
 * @brief 外部中断 ERU 通道3 与通道7 共用入口。
 *        通道3（P02_0）接摄像头场同步信号，触发 camera_vsync_handler()
 *        启动一帧 DMA 采集。通道7（P15_1）当前空置。
 */
IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    if(exti_flag_get(ERU_CH3_REQ6_P02_0))           // 通道3中断
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // 摄像头触发采集统一回调函数
    }
    if(exti_flag_get(ERU_CH7_REQ16_P15_1))          // 通道7中断
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** 外部中断函数 ****************************


// **************************** DMA中断函数 ****************************
/**
 * @brief 摄像头 DMA 采集完成中断（DMA 通道5）。
 *        一帧图像搬移结束后调用 camera_dma_handler()，置位帧完成标志，
 *        通知主循环开始视觉处理。
 */
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    camera_dma_handler();                           // 摄像头采集完成统一回调函数
}
// **************************** DMA中断函数 ****************************


// **************************** 串口中断函数 ****************************
// 串口0默认作为调试串口
/**
 * @brief 串口0 发送中断（当前空置）。
 */
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套



}
/**
 * @brief 串口0 接收中断（调试串口）。
 *        若开启 DEBUG_UART_USE_INTERRUPT，调用 debug_interrupr_handler()
 *        把数据读入 debug 环形缓冲区。
 */
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套

#if DEBUG_UART_USE_INTERRUPT                        // 如果开启 debug 串口中断
        debug_interrupr_handler();                  // 调用 debug 串口接收处理函数 数据会被 debug 环形缓冲区读取
#endif                                              // 如果修改了 DEBUG_UART_INDEX 那这段代码需要放到对应的串口中断去
}


// 串口1默认连接到摄像头配置串口
/**
 * @brief 串口1 发送中断（当前空置）。
 */
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套




}
/**
 * @brief 串口1 接收中断（摄像头配置串口）。
 *        调用 camera_uart_handler() 处理摄像头参数配置应答。
 */
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    camera_uart_handler();                          // 摄像头参数配置统一回调函数
}

// 串口2默认连接到无线转串口模块
/**
 * @brief 串口2 发送中断（当前空置）。
 */
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套



}

/**
 * @brief 串口2 接收中断（无线转串口模块）。
 *        调用 wireless_module_uart_handler() 处理上位机/遥控指令。
 */
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_wireless_rx_handler();                 // 无线模块统一回调函数



}
// 串口3默认连接到GPS定位模块
/**
 * @brief 串口3 发送中断（当前空置）。
 */
IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套



}

/**
 * @brief 串口3 接收中断（GNSS 定位模块）。
 *        调用 wireless_uart_callback() 解析 NMEA 报文。
 */
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    pal_gnss_rx_callback();                           // GNSS串口回调函数



}

/**
 * @brief 串口通讯错误中断集合。
 *        串口0~3 发生帧错/溢出错时，调用 SDK 的 IfxAsclin_Asc_isrError()
 *        统一处理，避免错误状态阻塞后续通讯。
 */
// 串口通讯错误中断
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);
}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);
}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
    pal_irq_global_ctrl(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);
}
