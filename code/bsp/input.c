/*
 * input.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 *
 * @brief 输入驱动 + 按键上升沿检测 + 拨码开关扫描
 *        面向对象封装：全部运行时状态收敛到 input_t 结构体
 *        内部函数以 input_t* 为第一参数，公开 API 包装单实例
 */

#include "input.h"

/* ===== 按键 / 拨码开关引脚映射（只读常量）===== */
static const pal_ch_t s_key_pins[4] = {
    PAL_PIN_KEY1, PAL_PIN_KEY2, PAL_PIN_KEY3, PAL_PIN_KEY4,
};
static const pal_ch_t s_dip_pins[4] = {
    PAL_PIN_DIP1, PAL_PIN_DIP2, PAL_PIN_DIP3, PAL_PIN_DIP4,
};

/* ===== 菜单状态（保留原全局变量）===== */
state_machine_enum state = IMAGE_VALUE;

/* ===== 输入对象（封装全部运行时状态）===== */
/* 单实例（全项目仅一组按键 + 拨码开关） */
static input_t s_input = { 0 };

/* ===== 内部操作（均以 input_t* 为第一参数）===== */

/**
 * @brief 扫描单个按键，更新电平采样与上升沿触发标志
 *        上升沿 = 当前释放(1) 且 上一次按下(0)
 * @param in  输入对象指针
 * @param idx 按键索引 0~3，对应 KEY1~KEY4
 */
static void input_scan_key(input_t *in, uint8_t idx)
{
    uint8_t prev = in->key_level[idx];
    uint8_t cur  = pal_gpio_read(s_key_pins[idx]);

    in->key_last_level[idx] = prev;
    in->key_level[idx]      = cur;
    in->key_pressed[idx]    = (cur && !prev) ? 1 : 0;
}

/**
 * @brief 扫描单个拨码开关（低电平 = ON）
 */
static void input_scan_dip(input_t *in, uint8_t idx)
{
    in->dip_switch[idx] = (pal_gpio_read(s_dip_pins[idx]) == 0) ? 1 : 0;
}

/* ===== 公开 API（包装单实例 s_input）===== */

void Input_Init(void)
{
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        pal_gpio_init(s_key_pins[i], PAL_GPIO_INPUT);
        pal_gpio_init(s_dip_pins[i], PAL_GPIO_INPUT);

        /* 上拉输入默认高电平（释放态） */
        s_input.key_level[i]      = 1;
        s_input.key_last_level[i] = 1;
        s_input.key_pressed[i]    = 0;
        s_input.dip_switch[i]     = 0;
    }
}

void Input_Scan(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++)
    {
        input_scan_key(&s_input, i);
        input_scan_dip(&s_input, i);
    }
}

uint8_t Input_GetPressedKey(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++)
    {
        if (s_input.key_level[i] == 0)
        {
            return (uint8_t)(i + 1);   /* 1~4 对应 KEY1~KEY4 */
        }
    }
    return 0;
}
