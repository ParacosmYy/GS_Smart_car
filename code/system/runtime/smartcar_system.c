/**
 * @file smartcar_system.c
 * @brief Smart car system runtime orchestration.
 */

#include "smartcar_system.h"

#include "control.h"
#include "platform/system/system_port.h"
#include "scheduler.h"
#include "smartcar_app.h"
#include "smartcar_board.h"

void SmartcarSystem_Boot(void)
{
    SystemPort_ClockInit();
    SystemPort_DebugInit();

    SmartcarBoard_InitDevices();
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
