#ifndef IRQ_PORT_ADAPTER_H_
#define IRQ_PORT_ADAPTER_H_

/**
 * @file irq_port_adapter.h
 * @brief IRQ port adapter —— 桥接 Platform Port 契约 与 System IRQ Router。
 *
 * 职责：
 *  1. 从 Impl 层读取 irq_port routes，扩展 route 表（追加 scheduler_events / tick_ms）。
 *  2. 将系统配置（event/tick 映射）合并后注册到 SmartcarIrqRouter。
 *  3. 通过 irq_port_register_dispatch() 注入分发回调。
 *
 * 本文件是 System 层模块，位于 code/system/irq/。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 IRQ port adapter。
 *
 * 处理步骤：
 *  1. 通过 irq_port_get_routes() 获取 Impl 端口路由表。
 *  2. 结合本 adapter 内置的 system 配置（event/tick 映射）构造完整路由表。
 *  3. 调用 SmartcarIrqRouter_Init() 注册路由。
 *  4. 通过 irq_port_register_dispatch() 注入分发回调。
 *
 * @return void : 无返回值。
 */
void IrqPortAdapter_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_PORT_ADAPTER_H_ */
