/*
 * buzzer.c
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 *
 * @brief 蜂鸣器驱动 + 非阻塞提示模式
 *        面向对象封装：全部运行时状态收敛到 buzzer_t 结构体
 *        内部函数以 buzzer_t* 为第一参数，公开 API 包装单实例
 */

#include "buzzer.h"
#include "config.h"

/* ===== 事件模式参数（只读常量）===== */
typedef struct {
    uint8_t beep_count;   /* 总蜂鸣次数       */
    uint8_t on_frames;    /* 每次蜂鸣持续帧数 */
    uint8_t off_frames;   /* 每次蜂鸣间隔帧数 */
} buzzer_pattern_t;

/* 事件 → 模式参数查找表 */
static const buzzer_pattern_t s_patterns[] = {
    [BUZZER_EVENT_NONE]      = { 0, 0,                     0 },
    [BUZZER_EVENT_RING]      = { 3, BUZZER_BEEP_SHORT_F,   BUZZER_GAP_F },
    [BUZZER_EVENT_CROSSROAD] = { 1, BUZZER_BEEP_LONG_F,    0 },
};

/* ===== 蜂鸣器对象（封装全部运行时状态）===== */
typedef struct {
    buzzer_pattern_t pattern;       /* 当前模式参数（触发时从 s_patterns 加载） */
    uint8_t          remaining;     /* 剩余蜂鸣次数                              */
    uint8_t          frame_cnt;     /* 当前阶段已过帧数                          */
    uint8_t          is_beeping;    /* 1 = 蜂鸣中, 0 = 间隔 / 空闲               */
    uint8_t          active;        /* 1 = 有模式在运行                          */
} buzzer_t;

/* 单实例（全项目仅一个蜂鸣器） */
static buzzer_t s_buzzer = { 0 };

/* ===== 内部状态转换（均以 buzzer_t* 为第一参数）===== */

/** @brief 开始一次蜂鸣 */
static void buzzer_start_beep(buzzer_t *bz)
{
    buzzer_on();
    bz->is_beeping = 1;
    bz->frame_cnt  = 0;
}

/** @brief 结束蜂鸣进入间隔，剩余次数递减 */
static void buzzer_enter_gap(buzzer_t *bz)
{
    buzzer_off();
    bz->is_beeping = 0;
    bz->frame_cnt  = 0;
    bz->remaining--;
}

/** @brief 全部完成，回到空闲 */
static void buzzer_finish(buzzer_t *bz)
{
    buzzer_off();
    bz->active     = 0;
    bz->is_beeping = 0;
    bz->frame_cnt  = 0;
    bz->remaining  = 0;
}

/** @brief 加载事件模式并启动第一次蜂鸣 */
static void buzzer_fire(buzzer_t *bz, buzzer_event_t event)
{
    bz->pattern   = s_patterns[event];
    bz->remaining = bz->pattern.beep_count;
    bz->active    = 1;
    buzzer_start_beep(bz);
}

/** @brief 帧驱动的状态推进（核心调度逻辑） */
static void buzzer_step(buzzer_t *bz)
{
    if (!bz->active)
    {
        return;
    }

    bz->frame_cnt++;

    if (bz->is_beeping && bz->frame_cnt >= bz->pattern.on_frames)
    {
        /* 蜂鸣阶段结束 */
        if (bz->remaining > 1)
        {
            buzzer_enter_gap(bz);   /* 还有剩余 → 进入间隔 */
        }
        else
        {
            buzzer_finish(bz);      /* 最后一次 → 结束     */
        }
    }
    else if (!bz->is_beeping && bz->frame_cnt >= bz->pattern.off_frames)
    {
        buzzer_start_beep(bz);      /* 间隔结束 → 下一次蜂鸣 */
    }
}

/* ===== 硬件初始化 ===== */

/**
 * @brief 蜂鸣器初始化
 *        将 P11_11 配置为 GPO 推挽输出，初始拉低
 */
void buzzer_init(void)
{
    gpio_init(P11_11, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

/* ===== 公开 API（包装单实例 s_buzzer）===== */

void Buzzer_Trigger(buzzer_event_t event)
{
    if (event == BUZZER_EVENT_NONE || s_buzzer.active)
    {
        return;
    }
    buzzer_fire(&s_buzzer, event);
}

void Buzzer_Tick(void)
{
    buzzer_step(&s_buzzer);
}

uint8_t Buzzer_IsBusy(void)
{
    return s_buzzer.active;
}
