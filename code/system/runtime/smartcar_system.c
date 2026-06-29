/**
 * @file smartcar_system.c
 * @brief Smart car system runtime orchestration.
 * @author GS_Mark
 *
 * @par 设计说明
 * Runtime 是启动组合根，负责把 SystemPort、Board、Service 和 App 按顺序装配起来。
 */

#include "smartcar_system.h"

#include "control.h"
#include "platform/system/system_port.h"
#include "scheduler.h"
#include "smartcar_app.h"
#include "smartcar_board.h"

/**
 * @brief 启动整车运行时。
 *
 * Steps:
 *   1. 初始化时钟和调试通道。
 *   2. 初始化板级设备和控制服务。
 *   3. 初始化调度器并注册 App 任务。
 *   4. 启动周期中断并完成多核同步。
 *
 * @return void。
 */
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

/**
 * @brief 执行一次系统主循环。
 *
 * Steps:
 *   1. 转发到 App 层，由协作调度器运行业务任务。
 *
 * @return void。
 */
void SmartcarSystem_RunOnce(void)
{
    SmartcarApp_RunOnce();
}
