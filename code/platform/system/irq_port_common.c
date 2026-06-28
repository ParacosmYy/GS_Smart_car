/**
 * @file irq_port_common.c
 * @brief 中断端口公共实现 —— setter + dispatch 转发。
 *
 * 本文件实现 irq_port.h 中声明的 setter 和 dispatch 函数。
 * 中断分发回调由 System 层（IrqPortAdapter）在启动时注册。
 */

#include "platform/system/irq_port.h"

/** @brief 静态分发回调指针（不对外暴露） */
static irq_port_dispatch_fn_t s_irq_dispatch = 0;

void irq_port_register_dispatch(irq_port_dispatch_fn_t fn)
{
    s_irq_dispatch = fn;
}

void irq_port_dispatch(irq_source_t source)
{
    if (s_irq_dispatch != 0)
    {
        s_irq_dispatch(source);
    }
}
