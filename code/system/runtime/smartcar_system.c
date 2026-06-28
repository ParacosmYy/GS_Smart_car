/**
 * @file smartcar_system.c
 * @brief Smart car system runtime orchestration.
 */

#include "smartcar_system.h"

#include "control.h"
#include "irq/irq_port_adapter.h"
#include "platform/interface/mcu_io_if.h"
#include "platform/interface/device_if.h"
#include "platform/system/system_port.h"
#include "platform/target/target_platform.h"
#include "scheduler.h"
#include "smartcar_app.h"
#include "smartcar_board.h"

/* ── 启动时验证所有 ops 已注册 ──────────────────────── */
static uint8_t Platform_ValidateOps(void)
{
    if (McuIo_IsReady() == 0U)  return 0U;
    if (Device_IsReady() == 0U) return 0U;
    return 1U;
}

void SmartcarSystem_Boot(void)
{
    SystemPort_ClockInit();
    SystemPort_DebugInit();

    TargetPlatform_RegisterAll();
    IrqPortAdapter_Init();
    SmartcarBoard_InitDevices();

    /* 校验 ops 完整性，失败则陷入死循环（可挂调试器观察） */
    while (Platform_ValidateOps() == 0U) { }

    Control_Init();
    scheduler_init();
    SmartcarApp_Init();
    SmartcarBoard_StartPeriodicIrq();

    SystemPort_CoreSync();
}

void SmartcarSystem_RunOnce(void)
{
    SmartcarApp_RunOnce();
}
