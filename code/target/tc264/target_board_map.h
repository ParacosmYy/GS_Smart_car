#ifndef TARGET_BOARD_MAP_H_
#define TARGET_BOARD_MAP_H_

/**
 * @file target_board_map.h
 * @brief TC264 板级引脚/通道物理映射表。
 * @author GS_Mark
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint32_t timer;
    uint32_t ch1_pin;
    uint32_t ch2_pin;
} tc264_encoder_map_t;

typedef struct
{
    uint32_t uart;
    uint32_t tx;
    uint32_t rx;
} tc264_uart_map_t;

extern const uint32_t            g_tc264_pwm_map[];
extern const uint32_t            g_tc264_gpio_map[];
extern const tc264_encoder_map_t g_tc264_encoder_map[];
extern const uint32_t            g_tc264_pit_map[];
extern const tc264_uart_map_t    g_tc264_uart_map[];

#ifdef __cplusplus
}
#endif

#endif /* TARGET_BOARD_MAP_H_ */
