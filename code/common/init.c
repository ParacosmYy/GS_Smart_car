/*
 * init.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 *
 * @brief System initialization implementation.
 *
 * init_all() initializes communication, encoders, motor/servo, display,
 * camera, keys, buzzer, gyro, wireless link and PID controllers in a fixed
 * boot order. pit_init_all() configures CCU60 periodic interrupt channels
 * with PIT_PERIOD_MS from config.h.
 */
#include "init.h"
#include "platform.h"
#include "config.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"
#include "control.h"

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
    // ---- 步骤 1：串口初始化 ----
    pal_uart_init(PAL_CH_UART_CAM, 115200);//摄像头串口，波特率 115200
    pal_uart_init(PAL_CH_UART_BT, 115200);//蓝牙串口，波特率 115200

    //  ---- 步骤 2：方向编码器初始化（双通道，分别接左右轮）----
    pal_encoder_init(PAL_CH_ENCODER_L);   // 左轮编码器，TIM2 定时器
    pal_encoder_init(PAL_CH_ENCODER_R);   // 右轮编码器，TIM4 定时器

    //  ---- 步骤 3：执行机构 ----
    Motor_Init();       // 直流电机驱动初始化（PWM 频率见 config.h MOTOR_PWM_HZ）
    Servo_Init();       // 舵机初始化（PWM 频率 50Hz，见 config.h SERVO_PWM_HZ）

    //  ---- 步骤 4：显示与图像采集 ----
    pal_disp_init();    // TFT180 LCD 显示屏初始化，用于实时调试画面
    pal_cam_init();     // MT9V03X 灰度摄像头初始化，逐帧采集赛道图像

    //  ---- 步骤 5：人机交互外设 ----
    Input_Init();       // 全部按键初始化，用于启停与模式切换
    buzzer_init();      // 蜂鸣器初始化，用于报警与提示

    //  ---- 步骤 6：传感器与无线通信 ----
    pal_gyro_init();        // ICM20602 六轴陀螺仪初始化，提供 Z 轴角速度
    pal_wireless_init();    // 无线串口初始化，用于上位机数据回传

    //  ---- 步骤 7：控制 Handler 初始化（内部装载 PID 增益）----
    Control_Init();
}

/**
 * @brief 初始化周期中断定时器（PIT）
 *
 * 启用两个 CCU60 通道，周期统一为 PIT_PERIOD_MS（默认 10ms）。
 * 通道 0 触发主控周期任务，通道 1 触发辅助任务（如陀螺仪采样）。
 */
void pit_init_all(void)
{
    pal_pit_init(PAL_CH_PIT_0, PIT_PERIOD_MS);
    pal_pit_init(PAL_CH_PIT_1, PIT_PERIOD_MS); //0.01s 执行一次
}
