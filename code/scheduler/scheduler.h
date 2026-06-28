/**
 * @file scheduler.h
 * @brief Lightweight cooperative task scheduler.
 *
 *        无抢占、无堆栈切换——主循环调用 scheduler_run() 驱动全部任务。
 *        每个任务可由「事件触发」或「周期触发」或两者兼有。
 *
 *        典型用法：
 *          scheduler_init();
 *          scheduler_add(SmartcarApp_TaskVision,  0,  EVT_CAM_FRAME);  // 事件触发
 *          scheduler_add(SmartcarApp_TaskControl, 10, 0);              // 10ms 周期
 *          while (1) { scheduler_run(); }
 */
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include "event.h"

#define SCH_MAX_TASKS 8   /* 最大注册任务数 */

/* 系统毫秒计数器（由 10ms PIT ISR 递增，调度器用于周期判断）*/
extern volatile uint32_t g_system_ms;

/* 任务函数原型：接收当前事件掩码，任务自行判断是否需要处理 */
typedef void (*task_fn_t)(event_mask_t events);

/* 任务描述符 */
typedef struct
{
    task_fn_t     fn;          /* 任务函数指针                    */
    uint32_t      period_ms;   /* 执行周期（ms），0=仅事件触发    */
    uint32_t      last_run;    /* 上次执行时间戳（ms）            */
    event_mask_t  trigger;     /* 触发事件掩码，0=纯周期触发      */
    uint8_t       active;      /* 1=激活，0=暂停                  */
} sch_task_t;

/**
 * @brief 初始化调度器，清空任务表
 */
void scheduler_init(void);

/**
 * @brief 注册一个任务
 * @param fn        任务函数
 * @param period_ms 执行周期（0=仅事件触发）
 * @param trigger   触发事件（0=纯周期触发）
 * @return 任务槽位索引（>=0），-1=表满
 */
int8_t scheduler_add(task_fn_t fn, uint32_t period_ms, event_mask_t trigger);

/**
 * @brief 调度器主循环：获取事件 → 遍历任务 → 到期/命中则执行
 *        在 while(1) 中反复调用
 */
void scheduler_run(void);

#endif /* SCHEDULER_H_ */
