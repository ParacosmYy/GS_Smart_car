/**
 * @file scheduler.h
 * @brief Lightweight cooperative task scheduler.
 *
 *        无抢占、无堆栈切换——主循环调用 Scheduler_Run() 驱动全部任务。
 *        每个任务可由「事件触发」或「周期触发」或两者兼有。
 *
 *        典型用法：
 *          Scheduler_Init();
 *          Scheduler_AddEx(SmartcarApp_TaskVision,  0,  EVT_CAM_FRAME,
 *                           SCHEDULER_TASK_PHASE_NORMAL_EVENT);
 *          Scheduler_AddEx(SmartcarApp_TaskControl, 10, 0,
 *                           SCHEDULER_TASK_PHASE_FAST_PERIODIC);
 *          while (1) { Scheduler_Run(); }
 */
#ifndef CODE_SCHEDULER_SCHEDULER_H_
#define CODE_SCHEDULER_SCHEDULER_H_

#include <stdint.h>
#include "event.h"

#define SCH_MAX_TASKS 8   /* 最大注册任务数 */

/* 任务函数原型：接收当前事件掩码，任务自行判断是否需要处理 */
typedef void (*task_fn_t)(event_mask_t events);

/* 调度阶段，枚举顺序即任务执行优先级 */
typedef enum
{
    SCHEDULER_TASK_PHASE_SENSOR_EVENT = 0,
    SCHEDULER_TASK_PHASE_FAST_PERIODIC,
    SCHEDULER_TASK_PHASE_NORMAL_EVENT,
    SCHEDULER_TASK_PHASE_SLOW_PERIODIC,
    SCHEDULER_TASK_PHASE_COUNT
} scheduler_task_phase_t;

/* 任务描述符 */
typedef struct
{
    task_fn_t              fn;          /* 任务函数指针                 */
    uint32_t               period_ms;   /* 执行周期（ms），0=仅事件触发 */
    uint32_t               last_run;    /* 上次执行时间戳（ms）         */
    event_mask_t           trigger;     /* 触发事件掩码，0=纯周期触发   */
    scheduler_task_phase_t phase;       /* 显式调度阶段                 */
    uint8_t                active;      /* 1=激活，0=暂停               */
} sch_task_t;

/**
 * @brief 初始化调度器，清空任务表
 */
void Scheduler_Init(void);

/**
 * @brief 从 ISR 上下文推进调度器时间基
 * @param elapsed_ms 本次中断经过的毫秒数
 * @note 时间戳为 uint32_t 毫秒计数，周期判断使用无符号减法，支持自然回绕。
 */
void Scheduler_AddTickFromIsr(uint32_t elapsed_ms);

/**
 * @brief 获取当前调度器时间戳
 * @return 当前系统毫秒数
 * @note 调用方不要用绝对大小比较时间戳，应使用 `(now - then) >= interval` 形式处理回绕。
 */
uint32_t Scheduler_GetNowMs(void);

/**
 * @brief 注册一个任务
 * @param fn        任务函数
 * @param period_ms 执行周期（0=仅事件触发）
 * @param trigger   触发事件（0=纯周期触发）
 * @return 任务槽位索引（>=0），-1=参数非法或表满
 */
int8_t Scheduler_Add(task_fn_t fn, uint32_t period_ms, event_mask_t trigger);

/**
 * @brief 注册一个指定调度阶段的任务
 * @param fn        任务函数
 * @param period_ms 执行周期（0=仅事件触发）
 * @param trigger   触发事件（0=纯周期触发）
 * @param phase     调度阶段
 * @return 任务槽位索引（>=0），-1=参数非法或表满
 */
int8_t Scheduler_AddEx(task_fn_t fn,
                       uint32_t period_ms,
                       event_mask_t trigger,
                       scheduler_task_phase_t phase);

/**
 * @brief 调度器主循环：获取事件 → 遍历任务 → 到期/命中则执行
 *        在 while(1) 中反复调用
 */
void Scheduler_Run(void);

#endif /* CODE_SCHEDULER_SCHEDULER_H_ */
