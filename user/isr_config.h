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
* 文件名称          isr_config
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

#ifndef _isr_config_h
#define _isr_config_h



//======================================================特别注意====================================================
// 中断优先级不能设置为相同值，所有中断优先级都必须设置为不一样的值
//======================================================特别注意====================================================

/*===========================================================================*/
/*  优先级分配总策略（数值越大优先级越高，范围 1-255，0 表示关闭中断）        */
/*                                                                           */
/*  分组          优先级区间   设计意图                                       */
/*  ----          ----------   --------------------------------------------   */
/*  DMA 搬运      60           摄像头帧完成必须最先响应，否则丢帧            */
/*  外部中断 EXTI 40-43        摄像头场同步等异步事件，时效性高              */
/*  定时器 PIT    30-33        编码器/陀螺仪周期采样，可短暂延迟              */
/*  串口 UART     10-21        通讯非实时，优先级最低，丢字节影响小          */
/*  DMA 服务 EXTI 5            ERU 通道2/6 走 DMA，仅 0-47 可选，给低值      */
/*                                                                           */
/*  调参原则：实时性强的（DMA、场同步）往上压，容忍抖动的（串口）往下放。     */
/*  改动任一优先级后，必须全局检查不能出现重复值。                            */
/*===========================================================================*/

// ISR_PRIORITY：    TC264具有255个中断优先级可以设置 1-255，0优先级表示不开启中断，255为最高优先级
// INT_SERVICE：     宏定义决定中断由谁处理，也称为服务提供者（在TC264中，中断被叫做服务），可设置范围IfxSrc_Tos_cpu0 IfxSrc_Tos_cpu1 IfxSrc_Tos_dma  不可设置为其他值
// 如果INT_SERVICE设置为IfxSrc_Tos_dma的话，ISR_PRIORITY的可设置范围则是0-47。

//================================PIT 定时器中断================================
// CCU60 通道0：编码器测速（10ms 周期），优先级 30
// CCU60 通道1：陀螺仪 Z 轴积分（10ms 周期），优先级 31，略低于编码器避免丢计数
// CCU61 通道0/1：预留扩展，优先级 32/33
//================================================PIT中断参数相关定义===============================================
#define CCU6_0_CH0_INT_SERVICE	IfxSrc_Tos_cpu0	    // 定义CCU6_0 PIT通道0中断服务类型，即中断是由谁响应处理 IfxSrc_Tos_cpu0 IfxSrc_Tos_cpu1 IfxSrc_Tos_dma  不可设置为其他值
#define CCU6_0_CH0_ISR_PRIORITY 30	                // 定义CCU6_0 PIT通道0中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样

#define CCU6_0_CH1_INT_SERVICE	IfxSrc_Tos_cpu0
#define CCU6_0_CH1_ISR_PRIORITY 31

#define CCU6_1_CH0_INT_SERVICE	IfxSrc_Tos_cpu0
#define CCU6_1_CH0_ISR_PRIORITY 32

#define CCU6_1_CH1_INT_SERVICE	IfxSrc_Tos_cpu0
#define CCU6_1_CH1_ISR_PRIORITY 33



//================================GPIO 外部中断================================
// ERU 通道成对共用一个中断函数，函数内靠标志位区分来源。
// 通道3（P02_0）接摄像头场同步，优先级 43 最高，保证帧起始不被错过。
// 通道2/6 走 DMA 服务（摄像头 PCLK 触发 DMA），优先级取 DMA 上限内的低值 5。
//================================================GPIO中断参数相关定义===============================================
// 通道0与通道4是公用一个中断函数 在中断内部通过标志位判断是谁触发的中断
#define EXTI_CH0_CH4_INT_SERVICE IfxSrc_Tos_cpu0	// 定义ERU通道0和通道4中断服务类型，即中断是由谁响应处理 IfxSrc_Tos_cpu0 IfxSrc_Tos_cpu1 IfxSrc_Tos_dma  不可设置为其他值
#define EXTI_CH0_CH4_INT_PRIO  	40	                // 定义ERU通道0和通道4中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样

// 通道1与通道5是公用一个中断函数 在中断内部通过标志位 判断是谁触发的中断
#define EXTI_CH1_CH5_INT_SERVICE IfxSrc_Tos_cpu0	// 定义ERU通道1和通道5中断服务类型，同上
#define EXTI_CH1_CH5_INT_PRIO  	41	                // 定义ERU通道1和通道5中断优先级 同上

// 通道2与通道6是公用一个中断函数 在中断内部通过标志位 判断是谁触发的中断
#define EXTI_CH2_CH6_INT_SERVICE IfxSrc_Tos_dma	    // 定义ERU通道2和通道6中断服务类型，同上
#define EXTI_CH2_CH6_INT_PRIO  	5	                // 定义ERU通道2和通道6中断优先级 可设置范围为0-47

// 通道3与通道7是公用一个中断函数 在中断内部通过标志位 判断是谁触发的中断
#define EXTI_CH3_CH7_INT_SERVICE IfxSrc_Tos_cpu0	// 定义ERU通道3和通道7中断服务类型，同上
#define EXTI_CH3_CH7_INT_PRIO  	43	                // 定义ERU通道3和通道7中断优先级 同上


//================================DMA 中断================================
// 摄像头帧搬运完成中断，优先级最高（60），一帧图像到位后立刻处理。
//===================================================DMA中断参数相关定义===============================================
#define	DMA_INT_SERVICE         IfxSrc_Tos_cpu0	    // ERU触发DMA中断服务类型，即中断是由谁响应处理 IfxSrc_Tos_cpu0 IfxSrc_Tos_cpu1 IfxSrc_Tos_dma  不可设置为其他值
#define DMA_INT_PRIO  	        60	                // ERU触发DMA中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样


//================================串口中断================================
// 串口0~3 均由 CPU0 处理。同一串口内 RX 高于 TX（接收优先，避免丢字节）。
// 错误中断 ER 比同口 TX/RX 略高，确保错误状态被及时清除不阻塞通讯。
// 串口分配：UART0=调试，UART1=摄像头配置，UART2=无线，UART3=GNSS。
//===================================================串口中断参数相关定义===============================================
#define	UART0_INT_SERVICE       IfxSrc_Tos_cpu0	    // 定义串口0中断服务类型，即中断是由谁响应处理 IfxSrc_Tos_cpu0 IfxSrc_Tos_cpu1 IfxSrc_Tos_dma  不可设置为其他值
#define UART0_TX_INT_PRIO       11	                // 定义串口0发送中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样
#define UART0_RX_INT_PRIO       10	                // 定义串口0接收中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样
#define UART0_ER_INT_PRIO       12	                // 定义串口0错误中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样

#define	UART1_INT_SERVICE       IfxSrc_Tos_cpu0
#define UART1_TX_INT_PRIO       13
#define UART1_RX_INT_PRIO       14
#define UART1_ER_INT_PRIO       15

#define	UART2_INT_SERVICE       IfxSrc_Tos_cpu0
#define UART2_TX_INT_PRIO       16
#define UART2_RX_INT_PRIO       17
#define UART2_ER_INT_PRIO       18

#define	UART3_INT_SERVICE       IfxSrc_Tos_cpu0
#define UART3_TX_INT_PRIO       19
#define UART3_RX_INT_PRIO       20
#define UART3_ER_INT_PRIO       21


#endif
