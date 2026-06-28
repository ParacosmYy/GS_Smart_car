/*
 * state_machine.h
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 */

#ifndef CODE_STATE_MACHINE_H_
#define CODE_STATE_MACHINE_H_

#include "platform.h"

extern uint8_t key_last_status[4] ;  // 上一次按键电平
extern uint8_t key_status[4] ;       // 当前按键电平
extern uint8_t key_flag[4];          // 按键上升沿触发标志
extern pal_ch_t key_gpio[4] ;        // KEY1~KEY4 对应的 PAL 引脚通道

extern uint8_t dip_switch_flag[4];   // 拨码开关状态，1=ON（低电平有效）


// 菜单状态机：切换不同调试显示页面
typedef enum state_machine_enum
{
    image_value = 0,   // 图像显示页
    servo_value,       // 舵机参数页
    motor_value,       // 电机参数页
    car_run_value,     // 行驶参数页
}state_machine_enum ;

extern state_machine_enum state;

/**
 * @brief 检测单个按键的上升沿触发
 */
void key_check(uint16_t key_index);

/**
 * @brief 查询当前被按下的按键编号（电平检测）
 * @return 1~4 对应 KEY1~KEY4，0 表示无按下
 */
uint8_t key_state_check(void);

/**
 * @brief 初始化4个按键GPIO为上拉输入
 */
void key_init_all(void);

/**
 * @brief 扫描全部4个按键，更新触发标志
 */
void key_check_all(void);

/**
 * @brief 初始化4位拨码开关GPIO为浮空输入
 */
void dip_swithc_init();

/**
 * @brief 扫描全部4位拨码开关，更新状态标志
 */
void dip_switch_check_all();


#endif /* CODE_STATE_MACHINE_H_ */
