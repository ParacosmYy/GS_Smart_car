/*
 * init.c
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 */
#include "zf_common_headfile.h"
#include "config.h"

void init_all(void)
{
    uint8_t i = 0;

    // 此处编写用户代码 例如外设初始化代码等
    uart_init(UART_1,115200,UART1_TX_P02_2,UART1_RX_P02_3);//摄像头串口
    uart_init(UART_3,115200,UART3_TX_P15_7,UART3_RX_P20_3);//蓝牙串口

    //  使用示例encoder_quad_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P00_7, TIM2_ENCODER_CH2_P00_8);// 使用T2定时器   P00_7引脚进行计数    计数方向使用P00_8引脚
    encoder_dir_init(TIM2_ENCODER,TIM2_ENCODER_CH1_P33_7,TIM2_ENCODER_CH2_P33_6);
    encoder_dir_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);

    motor_init();

    servo_init();

    tft180_init();
    mt9v03x_init();

    key_init_all();
    buzzer_init();

    icm20602_init();
    wireless_uart_init();

    PosPID_Init(&servo_pid, SERVO_PID_KP, SERVO_PID_KI, SERVO_PID_KD);

    IncPID_Init(&left_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
    IncPID_Init(&right_motor_pid, MOTOR_PID_KP, MOTOR_PID_KI, MOTOR_PID_KD);
}

void pit_init_all(void)
{
    pit_ms_init(CCU60_CH0, PIT_PERIOD_MS);
    pit_ms_init(CCU60_CH1, PIT_PERIOD_MS); //0.01s 执行一次
}
