/**
 * @file smartcar_system.c
 * @brief Smart car system runtime orchestration.
 *
 * System runtime is the composition root between SDK entry and application
 * logic. It owns boot order so App code only registers business tasks.
 */

#include "smartcar_system.h"

#include "platform.h"
#include "scheduler.h"
#include "smartcar_app.h"
#include "smartcar_board.h"

/**
 * @brief 执行系统启动编排。
 *
 * 处理步骤：
 *  1. 初始化系统时钟与调试串口。
 *  2. 初始化本车板级设备与业务 Handler。
 *  3. 初始化调度器并注册应用任务。
 *  4. 启动周期中断源，再等待多核同步。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarSystem_Boot(void)
{
    pal_sys_clock_init();
    pal_sys_debug_init();

    SmartcarBoard_InitDevices();
    scheduler_init();
    SmartcarApp_Init();
    SmartcarBoard_StartPeriodicIrq();

    pal_sys_core_sync();
}

/**
 * @brief 执行一次系统主循环。
 *
 * 处理步骤：
 *  1. 将主循环驱动委托给应用层。
 *  2. 保持 SDK entry 与具体 App 模块解耦。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarSystem_RunOnce(void)
{
    SmartcarApp_RunOnce();
}
