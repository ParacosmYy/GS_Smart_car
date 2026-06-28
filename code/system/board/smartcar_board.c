/**
 * @file smartcar_board.c
 * @brief Smart car board startup implementation.
 *
 * This module owns the product board initialization sequence. Moving to a new
 * MCU should replace the Impl/Vendor layer first; this board sequence changes
 * only when the product board devices or startup policy change.
 */

#include "smartcar_board.h"

#include "config.h"
#include "pal_camera.h"
#include "pal_comm.h"
#include "pal_display.h"
#include "pal_encoder.h"
#include "pal_imu.h"
#include "pal_pit.h"
#include "pal_uart.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"

/**
 * @brief 初始化全部板级设备与控制 Handler。
 *
 * 处理步骤：
 *  1. 先初始化通信口，便于后续调试与设备配置。
 *  2. 初始化编码器和执行机构，保证控制环反馈与输出都就绪。
 *  3. 初始化显示、摄像头、人机交互、陀螺仪和无线通信。
 *  4. 初始化陀螺仪和无线通信。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarBoard_InitDevices(void)
{
    pal_uart_init(PAL_CH_UART_CAM, 115200U);
    pal_uart_init(PAL_CH_UART_BT, 115200U);

    pal_encoder_init(PAL_CH_ENCODER_L);
    pal_encoder_init(PAL_CH_ENCODER_R);

    Motor_Init();
    Servo_Init();

    pal_disp_init();
    pal_cam_init();

    Input_Init();
    buzzer_init();

    pal_gyro_init();
    pal_wireless_init();

}

/**
 * @brief 启动周期中断源。
 *
 * 处理步骤：
 *  1. 启动编码器 10ms 采样 PIT。
 *  2. 启动陀螺仪 10ms tick PIT。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarBoard_StartPeriodicIrq(void)
{
    pal_pit_init(PAL_CH_PIT_0, PIT_PERIOD_MS);
    pal_pit_init(PAL_CH_PIT_1, PIT_PERIOD_MS);
}
