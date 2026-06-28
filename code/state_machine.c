/*
 * state_machine.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 */
#include "state_machine.h"

state_machine_enum state  = 0;

uint8_t key_last_status[4] = {1, 1, 1, 1};
uint8_t key_status[4] = {1, 1, 1, 1};
uint8_t key_flag[4] = {0, 0, 0, 0};
uint8_t key_gpio[4] = {KEY1, KEY2, KEY3, KEY4};

uint8_t dip_switch_flag[4] = {0,0,0,0};

void key_check(uint16_t key_index)
{
    key_last_status[key_index] = key_status[key_index];
    key_status[key_index] = gpio_get_level(key_gpio[key_index]);
    if(key_status[key_index] && !key_last_status[key_index])
        key_flag[key_index] = 1;
    else
        key_flag[key_index] = 0;
}

uint8 key_state_check(void)
{
    if(gpio_get_level(KEY1) == 0)
    {
        return 1 ;
    }
    else  if(gpio_get_level(KEY2) == 0)
    {
        return 2 ;
    }
    else  if(gpio_get_level(KEY3) == 0)
    {
        return 3 ;
    }
    else  if(gpio_get_level(KEY4) == 0)
    {
        return 4 ;
    }
    // 无按键按下时返回 0，避免未定义返回值导致上层读取随机/上一次寄存器残留值
    return 0;
}

void key_init_all(void)
{
    gpio_init(KEY1 , GPI , 1 , GPI_PULL_UP);
    gpio_init(KEY2 , GPI , 1 , GPI_PULL_UP);
    gpio_init(KEY3 , GPI , 1 , GPI_PULL_UP);
    gpio_init(KEY4 , GPI , 1 , GPI_PULL_UP);
}

void key_check_all(void)
{
    key_check(0);
    key_check(1);
    key_check(2);
    key_check(3);
}

//--------------------------------------------拨码开关----------------------------------------

void dip_swithc_init()
{
    gpio_init(P11_12 , GPI , 0 , GPI_FLOATING_IN);
    gpio_init(P10_1 , GPI , 0 , GPI_FLOATING_IN);
    gpio_init(P10_2 , GPI , 0 , GPI_FLOATING_IN);
    gpio_init(P10_3 , GPI , 0 , GPI_FLOATING_IN);
}

void dip_switch_check_all()
{
    //-------------switch 1
    if(gpio_get_level(P11_12) == 0)
    {
        dip_switch_flag[0] = 1;
    }
    else
    {
        dip_switch_flag[0] = 0;
    }
    //--------------switch2
    if(gpio_get_level(P10_1) == 0)
    {
        dip_switch_flag[1] = 1;
    }
    else
    {
        dip_switch_flag[1] = 0;
    }
    //--------------switc3
    if(gpio_get_level(P10_2) == 0)
    {
        dip_switch_flag[2] = 1;
    }
    else
    {
        dip_switch_flag[2] = 0;
    }
    //---------------switch4
    if(gpio_get_level(P10_3) == 0)
    {
        dip_switch_flag[3] = 1;
    }
    else
    {
        dip_switch_flag[3] = 0;
    }

}
