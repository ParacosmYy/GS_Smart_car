/*
 * input.c
 */
#include "input.h"

#include "smartcar_board_resources.h"
#include "platform/interface/mcu_io_if.h"

#define INPUT_KEY_COUNT 4U

static const uint16_t s_keys[INPUT_KEY_COUNT] =
{
    SMARTCAR_GPIO_KEY1,
    SMARTCAR_GPIO_KEY2,
    SMARTCAR_GPIO_KEY3,
    SMARTCAR_GPIO_KEY4,
};

static const uint16_t s_dips[INPUT_KEY_COUNT] =
{
    SMARTCAR_GPIO_DIP1,
    SMARTCAR_GPIO_DIP2,
    SMARTCAR_GPIO_DIP3,
    SMARTCAR_GPIO_DIP4,
};

void Input_Init(void)
{
    uint8_t i;

    for (i = 0U; i < INPUT_KEY_COUNT; i++)
    {
        McuIo_GpioInit(s_keys[i], MCUIO_GPIO_INPUT);
        McuIo_GpioInit(s_dips[i], MCUIO_GPIO_INPUT);
    }
}
