/*
 * buzzer.c
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 */

#include "buzzer.h"

    // 打开和关闭


/**
 * @brief 蜂鸣器初始化
 *         将 P11_11 配置为 GPO 推挽输出，初始拉低，确保蜂鸣器上电时处于关闭状态
 */
void buzzer_init(void)
{
    // 初始化（假设用的是 P11_11 这根脚）
    gpio_init(P11_11, GPO, GPIO_LOW, GPO_PUSH_PULL);  // 初始拉低，蜂鸣器关闭
}

