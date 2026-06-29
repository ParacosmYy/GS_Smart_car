/**
 * @file input.c
 * @brief 按键和拨码输入 BSP 初始化。
 * @author GS_Mark
 *
 * @par 设计说明
 * 输入资源由产品配置表给出，本模块只完成 GPIO 输入模式配置。
 */
#include "input.h"

#include "smartcar_board_resources.h"
#include "hal/hal.h"

#define INPUT_KEY_COUNT 4U

static const smartcar_hal_gpio_id_t s_keys[INPUT_KEY_COUNT] =
{
    SMARTCAR_GPIO_KEY1,
    SMARTCAR_GPIO_KEY2,
    SMARTCAR_GPIO_KEY3,
    SMARTCAR_GPIO_KEY4,
};

static const smartcar_hal_gpio_id_t s_dips[INPUT_KEY_COUNT] =
{
    SMARTCAR_GPIO_DIP1,
    SMARTCAR_GPIO_DIP2,
    SMARTCAR_GPIO_DIP3,
    SMARTCAR_GPIO_DIP4,
};

/**
 * @brief 初始化按键与拨码 GPIO。
 *
 * Steps:
 *   1. 遍历产品定义的四个按键资源。
 *   2. 同步初始化四个拨码输入资源。
 *
 * @return void。
 */
void Input_Init(void)
{
    uint8_t i;

    for (i = 0U; i < INPUT_KEY_COUNT; i++)
    {
        SmartcarHal_GpioInit(s_keys[i], SMARTCAR_HAL_GPIO_INPUT);
        SmartcarHal_GpioInit(s_dips[i], SMARTCAR_HAL_GPIO_INPUT);
    }
}
