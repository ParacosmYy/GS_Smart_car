/**
 * @file smartcar_board.c
 * @brief Smart car board startup。
 */

#include "smartcar_board.h"
#include "system/board/smartcar_board_resources.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"

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
    buzzer_init();
    Device_ImuInit();
    Device_WirelessInit();
}

void SmartcarBoard_StartPeriodicIrq(void)
{
    McuIo_PitInit(SMARTCAR_PIT_ENCODER_SAMPLE, 10U);
    McuIo_PitInit(SMARTCAR_PIT_GYRO_TICK, 10U);
}
