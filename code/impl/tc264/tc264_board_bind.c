/**
 * @file tc264_board_bind.c
 * @brief TC264 板级 binding —— 定义全部 TC264 ops 表并注册。
 *
 * 本文件只做绑定（binding），不做初始化（init）。
 * 初始化在 Lifecycle 阶段由 smartcar_board.c 统一处理。
 */

#include "platform/board/board_ops_if.h"
#include "platform/target/target_platform.h"
#include "tc264_board_map.h"

/* 外部 ops 表定义（各 tc264_*_ops.c）*/
extern const gpio_ops_t     g_tc264_gpio_ops;
extern const pwm_ops_t      g_tc264_pwm_ops;
extern const uart_ops_t     g_tc264_uart_ops;
extern const encoder_ops_t  g_tc264_encoder_ops;
extern const pit_ops_t      g_tc264_pit_ops;
extern const camera_ops_t   g_tc264_camera_ops;
extern const display_ops_t  g_tc264_display_ops;
extern const imu_ops_t      g_tc264_imu_ops;
extern const wireless_ops_t g_tc264_wireless_ops;
extern const key_ops_t      g_tc264_key_ops;

static const target_board_ops_t s_tc264_ops =
{
    .gpio     = &g_tc264_gpio_ops,
    .pwm      = &g_tc264_pwm_ops,
    .uart     = &g_tc264_uart_ops,
    .encoder  = &g_tc264_encoder_ops,
    .pit      = &g_tc264_pit_ops,
    .camera   = &g_tc264_camera_ops,
    .display  = &g_tc264_display_ops,
    .imu      = &g_tc264_imu_ops,
    .wireless = &g_tc264_wireless_ops,
    .key      = &g_tc264_key_ops,
};

void TargetPlatform_RegisterAll(void)
{
    Board_BindOps(&s_tc264_ops);
}
