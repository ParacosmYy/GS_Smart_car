#ifndef PAL_GPIO_H_
#define PAL_GPIO_H_

/**
 * @file pal_gpio.h
 * @brief PAL GPIO 接口。
 */

#include <stdint.h>
#include "pal_resources.h"

typedef enum
{
    PAL_GPIO_OUTPUT,
    PAL_GPIO_INPUT
} pal_gpio_mode_t;

void    pal_gpio_init(pal_gpio_id_t pin, pal_gpio_mode_t mode);
void    pal_gpio_high(pal_gpio_id_t pin);
void    pal_gpio_low(pal_gpio_id_t pin);
uint8_t pal_gpio_read(pal_gpio_id_t pin);

#endif /* PAL_GPIO_H_ */
