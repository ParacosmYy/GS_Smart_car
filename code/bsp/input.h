/*
 * input.h
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 *
 * @brief 输入驱动（4 按键 + 4 位拨码开关）
 *        面向对象封装：全部电平/触发状态收敛到 input_t 结构体
 *        内部函数以 input_t* 为第一参数，公开 API 包装单实例
 */

#ifndef CODE_INPUT_H_
#define CODE_INPUT_H_

#include "platform/mcu/pal_gpio.h"

/* ===== 菜单状态机（调试显示页切换，不属于 input_t）===== */
typedef enum {
    IMAGE_VALUE = 0,    /* 图像显示页 */
    SERVO_VALUE,        /* 舵机参数页 */
    MOTOR_VALUE,        /* 电机参数页 */
    CAR_RUN_VALUE,      /* 行驶参数页 */
} state_machine_enum;

/* 当前菜单状态（保留给上层显示模块切换页面） */
extern state_machine_enum state;

/* ===== 输入对象（封装全部按键与拨码开关运行时状态）===== */
/**
 * @brief 字段说明（上拉输入下，按下 = 低电平）：
 *        - key_level[]       当前电平采样：1=释放（默认），0=按下
 *        - key_last_level[]  上一次电平采样，用于上升沿检测
 *        - key_pressed[]     上升沿触发标志：按下后释放瞬间置 1，下次扫描自动清零
 *        - dip_switch[]      拨码开关状态：1=ON（低电平有效），0=OFF
 */
typedef struct {
    uint8_t key_level[4];       /* 当前电平采样 */
    uint8_t key_last_level[4];  /* 上一次电平采样 */
    uint8_t key_pressed[4];     /* 上升沿触发标志 */
    uint8_t dip_switch[4];      /* 拨码开关状态 */
} input_t;

/**
 * @brief 输入初始化
 *        配置 4 个按键 + 4 位拨码开关 GPIO 为输入，状态清零
 */
void Input_Init(void);

/**
 * @brief 扫描全部按键与拨码开关（单实例包装）
 *        更新电平采样、上升沿触发标志、拨码开关状态
 *        需在主循环周期性调用
 */
void Input_Scan(void);

/**
 * @brief 查询当前被按下的按键编号（电平检测）
 * @return 1~4 对应 KEY1~KEY4，0 表示无按键按下
 */
uint8_t Input_GetPressedKey(void);

#endif /* CODE_INPUT_H_ */
