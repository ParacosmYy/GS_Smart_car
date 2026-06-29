/**
 * @file buzzer.c
 * @brief 有源蜂鸣器 BSP 时序实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 蜂鸣器只暴露事件触发和 10ms tick 推进接口；业务层不直接操作 GPIO。
 * 内部通过静态模式表描述圆环、十字路口等提示音，避免在上层散落时序常量。
 */
#include "buzzer.h"

#include "config.h"
#include "smartcar_board_resources.h"
#include "platform/interface/mcu_io_if.h"

typedef struct
{
    uint8_t beep_count;
    uint8_t on_ticks;
    uint8_t off_ticks;
} buzzer_pattern_t;

typedef struct
{
    buzzer_pattern_t pattern;
    uint8_t remaining;
    uint8_t tick_count;
    uint8_t is_beeping;
    uint8_t active;
} buzzer_t;

static const buzzer_pattern_t s_patterns[] =
{
    [BUZZER_EVENT_NONE] = {0U, 0U, 0U},
    [BUZZER_EVENT_RING] = {3U, BUZZER_BEEP_SHORT_TICKS, BUZZER_GAP_TICKS},
    [BUZZER_EVENT_CROSSROAD] = {1U, BUZZER_BEEP_LONG_TICKS, 0U},
};

static buzzer_t s_buzzer = {0};

/**
 * @brief 启动当前蜂鸣阶段。
 *
 * Steps:
 *   1. 拉高蜂鸣器 GPIO。
 *   2. 标记当前处于鸣叫阶段。
 *   3. 清零阶段 tick 计数。
 *
 * @param[in,out] p_buzzer 蜂鸣器状态对象。
 * @return void。
 */
static void start_beep(buzzer_t *p_buzzer)
{
    McuIo_GpioHigh(SMARTCAR_GPIO_BUZZER);
    p_buzzer->is_beeping = 1U;
    p_buzzer->tick_count = 0U;
}

/**
 * @brief 进入两次短鸣之间的静音间隔。
 *
 * Steps:
 *   1. 拉低蜂鸣器 GPIO。
 *   2. 标记当前处于间隔阶段。
 *   3. 消耗一次剩余鸣叫次数。
 *
 * @param[in,out] p_buzzer 蜂鸣器状态对象。
 * @return void。
 */
static void enter_gap(buzzer_t *p_buzzer)
{
    McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);
    p_buzzer->is_beeping = 0U;
    p_buzzer->tick_count = 0U;
    p_buzzer->remaining--;
}

/**
 * @brief 结束当前蜂鸣模式。
 *
 * Steps:
 *   1. 拉低蜂鸣器 GPIO。
 *   2. 清除 active 与 is_beeping 状态。
 *
 * @param[in,out] p_buzzer 蜂鸣器状态对象。
 * @return void。
 */
static void finish_beep(buzzer_t *p_buzzer)
{
    McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);
    p_buzzer->active = 0U;
    p_buzzer->is_beeping = 0U;
}

/**
 * @brief 装载并启动指定蜂鸣模式。
 *
 * Steps:
 *   1. 从静态模式表复制事件对应的蜂鸣参数。
 *   2. 初始化剩余次数并置 active。
 *   3. 立即进入第一次鸣叫阶段。
 *
 * @param[in,out] p_buzzer 蜂鸣器状态对象。
 * @param[in] event 业务反馈事件。
 * @return void。
 */
static void fire_pattern(buzzer_t *p_buzzer, buzzer_event_t event)
{
    p_buzzer->pattern = s_patterns[event];
    p_buzzer->remaining = p_buzzer->pattern.beep_count;
    p_buzzer->active = 1U;
    start_beep(p_buzzer);
}

/**
 * @brief 推进蜂鸣器状态机一个 10ms tick。
 *
 * Steps:
 *   1. 空闲时直接返回。
 *   2. 鸣叫阶段达到 on_ticks 后进入间隔或结束。
 *   3. 间隔阶段达到 off_ticks 后启动下一次鸣叫。
 *
 * @param[in,out] p_buzzer 蜂鸣器状态对象。
 * @return void。
 */
static void step_pattern(buzzer_t *p_buzzer)
{
    if (p_buzzer->active == 0U)
    {
        return;
    }

    p_buzzer->tick_count++;

    if ((p_buzzer->is_beeping != 0U) && (p_buzzer->tick_count >= p_buzzer->pattern.on_ticks))
    {
        if (p_buzzer->remaining > 1U)
        {
            enter_gap(p_buzzer);
        }
        else
        {
            finish_beep(p_buzzer);
        }
    }
    else if ((p_buzzer->is_beeping == 0U) && (p_buzzer->tick_count >= p_buzzer->pattern.off_ticks))
    {
        start_beep(p_buzzer);
    }
}

/**
 * @brief 初始化蜂鸣器 GPIO。
 *
 * Steps:
 *   1. 将产品资源表中的蜂鸣器引脚配置为输出。
 *
 * @return void。
 */
void buzzer_init(void)
{
    McuIo_GpioInit(SMARTCAR_GPIO_BUZZER, MCUIO_GPIO_OUTPUT);
}

/**
 * @brief 触发一次蜂鸣提示。
 *
 * Steps:
 *   1. 忽略空事件。
 *   2. 当前蜂鸣器忙时不抢占已有提示。
 *   3. 空闲时装载事件对应的蜂鸣模式。
 *
 * @param[in] event 蜂鸣事件。
 * @return void。
 */
void Buzzer_Trigger(buzzer_event_t event)
{
    if ((event != BUZZER_EVENT_NONE) && (s_buzzer.active == 0U))
    {
        fire_pattern(&s_buzzer, event);
    }
}

/**
 * @brief 推进蜂鸣器周期时序。
 *
 * Steps:
 *   1. 调用内部状态机推进一个 10ms tick。
 *
 * @return void。
 */
void Buzzer_Tick(void)
{
    step_pattern(&s_buzzer);
}

/**
 * @brief 查询蜂鸣器是否正在执行提示。
 *
 * @return 1 表示忙；0 表示空闲。
 */
uint8_t Buzzer_IsBusy(void)
{
    return s_buzzer.active;
}
