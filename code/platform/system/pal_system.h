#ifndef PAL_SYSTEM_H_
#define PAL_SYSTEM_H_

/**
 * @file pal_system.h
 * @brief PAL 系统与中断控制接口。
 */

#include <stdint.h>

void     pal_sys_clock_init(void);
void     pal_sys_debug_init(void);
void     pal_sys_core_sync(void);
uint32_t pal_irq_global_disable(void);
void     pal_irq_global_restore(uint32_t state);
void     pal_irq_global_ctrl(uint8_t state);

#endif /* PAL_SYSTEM_H_ */
