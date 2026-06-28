/*
 * init.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 *
 *  系统初始化文件
 *  init_all()  顺序初始化全部外设：通信 → 编码器 → 电机/舵机 →
 *  显示与图像 → 按键/蜂鸣器 → 陀螺仪 → PID 控制器。
 *  pit_init_all() 配置两个周期中断定时器（CCU60 通道 0/1），
 *  周期由 config.h 中的 PIT_PERIOD_MS 决定（默认 10ms）。
 */
#include "zf_common_headfile.h"
#include "config.h"

/**
 * @brief 初始化全部外设与 PID 控制器
 *
 * 调用顺序需保持稳定：
 *  1. 串口先行，便于后续调试日志输出；
 *  2. 编码器与电机/舵机在前，确保控制环有反馈量；
 *  3. 显示与摄像头随后，保证视觉流水线可立即采集；
 *  4. 最后初始化陀螺仪、无线串口与 PID 控制器。
 */
void init_all(void)
{
    uint8_t i = 0;

    // ---- 步骤 1：串口初始化 ----
    uart_init(UART_1,115200,UART1_TX_P02_2,UART1_RX_P02_3);//摄像头串口，波特率 115200
    uart_init(UART_3,115200,UART3_TX_P15_7,UART3_RX_P20_3);//蓝牙串口，波特率 115200

    //  ---- 步骤 2：方向编码器初始化（双通道，分别接左右轮）----
    //  使用示例encoder_quad_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P00_7, TIM2_ENCODER_CH2_P00_8);// 使用T2定时器   P00_7引脚进行计数    计数方向使用P00_8引脚
    encoder_dir_init(TIM2_ENCODER,TIM2_ENCODER_CH1_P33_7,TIM2_ENCODER_CH2_P33_6);   // 左轮编码器，TIM2 定时器
    encoder_dir_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);   // 右轮编码器，TIM4 定时器

    //  ---- 步骤 3：执行机构 ----
    motor_init();       // 直流电机驱动初始化（PWM 频率见 config.h MOTOR_PWM_HZ）
    servo_init();       // 舵机初始化（PWM 频率 50Hz，见 config.h SERVO_PWM_HZ）

    //  ---- 步骤 4：显示与图像采集 ----
    tft180_init();      // TFT180 LCD 显示屏初始化，用于实时调试画面
    mt9v03x_init();     // MT9V03X 灰度摄像头初始化，逐帧采集赛道图像

    //  ---- 步骤 5：人机交互外设 ----
    key_init_all();     // 全部按键初始化，用于启停与模式切换
    buzzer_init();      // 蜂鸣器初始化，用于报警与提示

    //  ---- 步骤 6：传感器与无线通信 ----
    icm20602_init();        // ICM20602 六轴陀螺仪初始化，提供 Z 轴角速度
    wireless_uart_init();  // 无线串口初始化，用于上位机数据回传

    //  ---- 步骤 7：PID 控制器参数装载（增益来自 config.h）----
    PosPID_Init(&servo_pid, SERVO_PID_KP, SERVO_PID_KI, SERVO_PID_KD);              // 舵机位置式 PID

    IncPID_Init(&left_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);          // 左电机增量式 PID
    IncPID_Init(&right_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);         // 右电机增量式 PID
}

/**
 * @brief 初始化周期中断定时器（PIT）
 *
 * 启用两个 CCU60 通道，周期统一为 PIT_PERIOD_MS（默认 10ms）。
 * 通道 0 触发主控周期任务，通道 1 触发辅助任务（如陀螺仪采样）。
 */
void pit_init_all(void)
{
    pit_ms_init(CCU60_CH0, PIT_PERIOD_MS);
    pit_ms_init(CCU60_CH1, PIT_PERIOD_MS); //0.01s 执行一次
}
