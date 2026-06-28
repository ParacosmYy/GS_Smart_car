#ifndef PLATFORM_BOARD_OPS_IF_H_
#define PLATFORM_BOARD_OPS_IF_H_

/**
 * @file board_ops_if.h
 * @brief Platform 板级 ops 注册表契约。
 *
 * 本文件定义 target_board_ops_t 结构体，是 Board 层装配产品 ops 的契约。
 * Impl 层（如 tc264_board_bind.c）填充 ops 实例后调用 Board_BindOps()。
 * Board 层（smartcar_board.c）将注册表分发到 Platform dispatch 层。
 *
 * 类型定义放在 Platform 层，Impl 和 Board 都依赖此头文件。
 */

#include "platform/interface/mcu_io_if.h"
#include "platform/interface/device_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 目标板级 ops 注册表。
 *
 * 每种外设一个 ops 指针，Impl 层填充，Board 层消费。
 */
typedef struct
{
    const gpio_ops_t     *gpio;
    const pwm_ops_t      *pwm;
    const uart_ops_t     *uart;
    const encoder_ops_t  *encoder;
    const pit_ops_t      *pit;
    const camera_ops_t   *camera;
    const display_ops_t  *display;
    const imu_ops_t      *imu;
    const wireless_ops_t *wireless;
    const key_ops_t      *key;
} target_board_ops_t;

/**
 * @brief 绑定板级 ops 注册表（Board 层实现，Impl 层调用）。
 *
 * @param p_ops 非空 ops 表指针（生命周期须持续到系统关机）。
 */
void Board_BindOps(const target_board_ops_t *p_ops);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_BOARD_OPS_IF_H_ */
