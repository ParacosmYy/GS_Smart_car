/*
 * smartcar_app.c
 * @brief Smart car application scheduler implementation.
 *
 * Processing flow:
 * ISR entries do minimum bounded work and task processing is dispatched by
 * the cooperative scheduler. App owns only lifecycle entrypoints.
 */
#include "smartcar_app.h"
#include "scheduler.h"
#include "service/smartcar_tasks.h"

//******************************** Variables ********************************//
static uint8_t s_task_register_fail_count = 0U;
//******************************** Variables ********************************//

/**
 * @brief 应用层初始化入口。
 *
 * 处理步骤：
 *  1. 清零任务注册诊断计数。
 *  2. 通过 Service task registry 注册业务任务。
 *
 * @return void : 无返回值。
 *
 * */
void SmartcarApp_Init(void)
{
    s_task_register_fail_count = SmartcarTasks_RegisterAll();
}

/**
 * @brief 获取应用任务注册失败次数。
 *
 * @return uint8_t : 注册失败次数。
 *
 * */
uint8_t SmartcarApp_GetTaskRegisterFailCount(void)
{
    return s_task_register_fail_count;
}

/**
 * @brief 应用层单次循环
 *        由调度器分发 Service 任务。
 */
void SmartcarApp_RunOnce(void)
{
    scheduler_run();
}
