#ifndef PAL_UART_H_
#define PAL_UART_H_

/**
 * @file pal_uart.h
 * @brief PAL UART 接口。
 */

#include <stdint.h>
#include "platform/common/pal_resources.h"

void pal_uart_init(pal_uart_id_t ch, uint32_t baud);

#endif /* PAL_UART_H_ */
