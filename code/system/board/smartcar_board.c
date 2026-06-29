/**
 * @file smartcar_board.c
 * @brief Smart car board startup。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件集中完成产品级设备初始化顺序，不向上暴露具体 TC264 引脚或 Vendor API。
 */

#include "smartcar_board.h"
#include "config.h"
#include "platform/port_if.h"
#include "smartcar_board_resources.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"

/**
 * @brief 初始化整车使用的板级设备。
 *
 * Steps:
 *   1. 初始化通信串口和编码器采样通道。
 *   2. 初始化执行器、显示、摄像头、输入、蜂鸣器、IMU 和无线模块。
 *
 * @return void。
 */
void SmartcarBoard_InitDevices(void)
{
    McuIo_UartInit(SMARTCAR_UART_CAMERA, 115200U);
    McuIo_UartInit(SMARTCAR_UART_BT, 115200U);
    McuIo_EncoderInit(SMARTCAR_ENCODER_LEFT);
    McuIo_EncoderInit(SMARTCAR_ENCODER_RIGHT);
    Motor_Init();
    Servo_Init();
    Device_DisplayInit();
    Device_CameraInit();
    Input_Init();
    Buzzer_Init();
    Device_ImuInit();
    Device_WirelessInit();
}

/**
 * @brief 启动周期中断源。
 *
 * Steps:
 *   1. 按 PIT_PERIOD_MS 初始化编码器采样 PIT。
 *   2. 按 PIT_PERIOD_MS 初始化陀螺仪 tick PIT。
 *
 * @return void。
 */
void SmartcarBoard_StartPeriodicIrq(void)
{
    McuIo_PitInit(SMARTCAR_PIT_ENCODER_SAMPLE, PIT_PERIOD_MS);
    McuIo_PitInit(SMARTCAR_PIT_GYRO_TICK, PIT_PERIOD_MS);
}
