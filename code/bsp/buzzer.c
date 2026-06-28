/*
 * buzzer.c
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 */

#include "buzzer.h"
#include "config.h"

/* ===== 蜂鸣器模式定义 ===== */
/* 每种事件对应不同蜂鸣模式：次数 + 每次持续帧数 + 间隔帧数 */
typedef struct {
    uint8_t beep_count;    /* 总蜂鸣次数            */
    uint8_t on_frames;     /* 每次蜂鸣持续帧数      */
    uint8_t off_frames;    /* 每次蜂鸣间隔帧数      */
} buzzer_pattern_t;

/* 模式查找表，索引对应 buzzer_event_t 枚举值 */
static const buzzer_pattern_t s_patterns[] = {
    { 0, 0,                      0 },              /* BUZZER_EVENT_NONE      */
    { 3, BUZZER_BEEP_SHORT_F,    BUZZER_GAP_F },   /* BUZZER_EVENT_RING      */
    { 1, BUZZER_BEEP_LONG_F,     0 },              /* BUZZER_EVENT_CROSSROAD */
};

/* ===== 非阻塞状态机内部状态 ===== */
static buzzer_event_t s_current_event  = BUZZER_EVENT_NONE;
static uint8_t        s_remaining_beeps = 0;
static uint8_t        s_frame_counter   = 0;
static uint8_t        s_is_on           = 0;

    // 打开和关闭


/**
 * @brief 蜂鸣器初始化
 *         将 P11_11 配置为 GPO 推挽输出，初始拉低，确保蜂鸣器上电时处于关闭状态
 */
void buzzer_init(void)
{
    gpio_init(P11_11, GPO, GPIO_LOW, GPO_PUSH_PULL);
}

/* ===== 非阻塞蜂鸣器模式实现 ===== */

void Buzzer_Trigger(buzzer_event_t event)
{
    if (event == BUZZER_EVENT_NONE)
    {
        return;
    }
    /* 正在播放时不接受新请求，避免覆盖当前模式 */
    if (s_current_event != BUZZER_EVENT_NONE)
    {
        return;
    }

    s_current_event   = event;
    s_remaining_beeps = s_patterns[event].beep_count;
    s_frame_counter   = 0;
    s_is_on           = 1;
    buzzer_on();
}

void Buzzer_Tick(void)
{
    if (s_current_event == BUZZER_EVENT_NONE)
    {
        return;
    }

    s_frame_counter++;

    if (s_is_on)
    {
        /* 蜂鸣中：检查是否到达持续帧数 */
        if (s_frame_counter >= s_patterns[s_current_event].on_frames)
        {
            buzzer_off();
            s_is_on         = 0;
            s_frame_counter = 0;
            s_remaining_beeps--;

            if (s_remaining_beeps == 0)
            {
                /* 所有蜂鸣完成，回到空闲 */
                s_current_event = BUZZER_EVENT_NONE;
            }
        }
    }
    else
    {
        /* 间隔中：检查是否到达间隔帧数 */
        if (s_frame_counter >= s_patterns[s_current_event].off_frames)
        {
            buzzer_on();
            s_is_on         = 1;
            s_frame_counter = 0;
        }
    }
}

uint8_t Buzzer_IsBusy(void)
{
    return (s_current_event != BUZZER_EVENT_NONE) ? 1 : 0;
}

