#ifndef PAL_SYSTEM_H_
#define PAL_SYSTEM_H_

/**
 * @file pal_system.h
 * @brief LEGACY 系统端口兼容接口。
 */

#include "system_port.h"

#define pal_sys_clock_init      SystemPort_ClockInit
#define pal_sys_debug_init      SystemPort_DebugInit
#define pal_sys_core_sync       SystemPort_CoreSync
#define pal_irq_global_disable  SystemPort_IrqGlobalDisable
#define pal_irq_global_restore  SystemPort_IrqGlobalRestore
#define pal_irq_global_ctrl     SystemPort_IrqGlobalCtrl

#endif /* PAL_SYSTEM_H_ */
