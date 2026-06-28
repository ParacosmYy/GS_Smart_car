/*
 * state_machine.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 */
#include "input.h"

// 当前菜单状态（图像/舵机/电机/行驶参数）
state_machine_enum state  = 0;

// 按键状态数组：上拉输入下，1=释放（默认），0=按下
uint8_t key_last_status[4] = {1, 1, 1, 1};   // 上一次按键电平
uint8_t key_status[4] = {1, 1, 1, 1};        // 当前按键电平
uint8_t key_flag[4] = {0, 0, 0, 0};          // 按键触发标志（上升沿瞬间置1）
pal_ch_t key_gpio[4] = {PAL_PIN_KEY1, PAL_PIN_KEY2, PAL_PIN_KEY3, PAL_PIN_KEY4}; // KEY1~KEY4 对应的 PAL 引脚通道

uint8_t dip_switch_flag[4] = {0,0,0,0};      // 拨码开关状态，1=拨到ON（低电平有效）

/**
 * @brief 检测单个按键的上升沿（按下后释放瞬间触发一次）
 * @param key_index 按键索引 0~3，对应 KEY1~KEY4
 */
void key_check(uint16_t key_index)
{
    key_last_status[key_index] = key_status[key_index];
    key_status[key_index] = pal_gpio_read(key_gpio[key_index]);
    // 上升沿：当前为高（释放），上一次为低（按下），置位触发标志
    if(key_status[key_index] && !key_last_status[key_index])
        key_flag[key_index] = 1;
    else
        key_flag[key_index] = 0;
}

/**
 * @brief 查询当前被按下的按键编号（电平检测，非边沿）
 * @return 1~4 对应 KEY1~KEY4，0 表示无按键按下
 */
uint8_t key_state_check(void)
{
    if(pal_gpio_read(PAL_PIN_KEY1) == 0)
    {
        return 1 ;
    }
    else  if(pal_gpio_read(PAL_PIN_KEY2) == 0)
    {
        return 2 ;
    }
    else  if(pal_gpio_read(PAL_PIN_KEY3) == 0)
    {
        return 3 ;
    }
    else  if(pal_gpio_read(PAL_PIN_KEY4) == 0)
    {
        return 4 ;
    }
    // 无按键按下时返回 0，避免未定义返回值导致上层读取随机/上一次寄存器残留值
    return 0;
}

/**
 * @brief 初始化4个按键GPIO为上拉输入
 */
void key_init_all(void)
{
    pal_gpio_init(PAL_PIN_KEY1 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_KEY2 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_KEY3 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_KEY4 , PAL_GPIO_INPUT);
}

/**
 * @brief 扫描全部4个按键，更新各自的触发标志
 */
void key_check_all(void)
{
    key_check(0);
    key_check(1);
    key_check(2);
    key_check(3);
}

//--------------------------------------------拨码开关----------------------------------------

/**
 * @brief 初始化4位拨码开关GPIO为输入
 *        PAL 通道：PAL_PIN_DIP1~4（物理映射在 platform_<mcu>.c）
 */
void dip_swithc_init()
{
    pal_gpio_init(PAL_PIN_DIP1 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_DIP2 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_DIP3 , PAL_GPIO_INPUT);
    pal_gpio_init(PAL_PIN_DIP4 , PAL_GPIO_INPUT);
}

/**
 * @brief 扫描全部4位拨码开关，更新 dip_switch_flag
 *        低电平（拨到ON）置1，高电平置0
 */
void dip_switch_check_all()
{
    //-------------switch 1
    if(pal_gpio_read(PAL_PIN_DIP1) == 0)
    {
        dip_switch_flag[0] = 1;
    }
    else
    {
        dip_switch_flag[0] = 0;
    }
    //--------------switch2
    if(pal_gpio_read(PAL_PIN_DIP2) == 0)
    {
        dip_switch_flag[1] = 1;
    }
    else
    {
        dip_switch_flag[1] = 0;
    }
    //--------------switc3
    if(pal_gpio_read(PAL_PIN_DIP3) == 0)
    {
        dip_switch_flag[2] = 1;
    }
    else
    {
        dip_switch_flag[2] = 0;
    }
    //---------------switch4
    if(pal_gpio_read(PAL_PIN_DIP4) == 0)
    {
        dip_switch_flag[3] = 1;
    }
    else
    {
        dip_switch_flag[3] = 0;
    }

}
