/*
 * buzzer.h
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 */

#ifndef CODE_BUZZER_H_
#define CODE_BUZZER_H_

#include "zf_common_headfile.h"

/**
 * @brief 打开蜂鸣器（P11_11 拉高）
 */
static inline void buzzer_on(void)  { gpio_high(P11_11); }

/**
 * @brief 关闭蜂鸣器（P11_11 拉低）
 */
static inline void buzzer_off(void) { gpio_low(P11_11);  }

/* ===== 非阻塞蜂鸣器模式 API ===== */
/* 在主循环中调用，不阻塞控制流程                          */

typedef enum {
    BUZZER_EVENT_NONE = 0,
    BUZZER_EVENT_RING,        /* 圆环检测：3 声短促（~100ms × 3）   */
    BUZZER_EVENT_CROSSROAD,   /* 十字路口检测：1 声长鸣（~400ms）    */
} buzzer_event_t;

/**
 * @brief 触发一次蜂鸣器提示模式（非阻塞）
 *        如果当前正在播放，则忽略新请求
 * @param event 事件类型（圆环 / 十字路口）
 */
void Buzzer_Trigger(buzzer_event_t event);

/**
 * @brief 蜂鸣器时序驱动，需在主循环每帧调用一次
 *        内部帧计数器控制蜂鸣/间隔/结束
 */
void Buzzer_Tick(void);

/**
 * @brief 查询蜂鸣器是否正在播放
 * @return 1=忙（正在播放），0=空闲
 */
uint8_t Buzzer_IsBusy(void);

/**
 * @brief 蜂鸣器初始化，配置 P11_11 为 GPO 推挽输出并拉低
 */
void buzzer_init(void);

#endif /* CODE_BUZZER_H_ */
