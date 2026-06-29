/*
 * buzzer.c
 */
#include "buzzer.h"

#include "config.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"

typedef struct
{
    uint8_t beep_count;
    uint8_t on_frames;
    uint8_t off_frames;
} buzzer_pattern_t;

typedef struct
{
    buzzer_pattern_t pattern;
    uint8_t remaining;
    uint8_t frame_count;
    uint8_t is_beeping;
    uint8_t active;
} buzzer_t;

static const buzzer_pattern_t s_patterns[] =
{
    [BUZZER_EVENT_NONE] = {0U, 0U, 0U},
    [BUZZER_EVENT_RING] = {3U, BUZZER_BEEP_SHORT_F, BUZZER_GAP_F},
    [BUZZER_EVENT_CROSSROAD] = {1U, BUZZER_BEEP_LONG_F, 0U},
};

static buzzer_t s_buzzer = {0};

static void start_beep(buzzer_t *p_buzzer)
{
    McuIo_GpioHigh(SMARTCAR_GPIO_BUZZER);
    p_buzzer->is_beeping = 1U;
    p_buzzer->frame_count = 0U;
}

static void enter_gap(buzzer_t *p_buzzer)
{
    McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);
    p_buzzer->is_beeping = 0U;
    p_buzzer->frame_count = 0U;
    p_buzzer->remaining--;
}

static void finish_beep(buzzer_t *p_buzzer)
{
    McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);
    p_buzzer->active = 0U;
    p_buzzer->is_beeping = 0U;
}

static void fire_pattern(buzzer_t *p_buzzer, buzzer_event_t event)
{
    p_buzzer->pattern = s_patterns[event];
    p_buzzer->remaining = p_buzzer->pattern.beep_count;
    p_buzzer->active = 1U;
    start_beep(p_buzzer);
}

static void step_pattern(buzzer_t *p_buzzer)
{
    if (p_buzzer->active == 0U)
    {
        return;
    }

    p_buzzer->frame_count++;

    if ((p_buzzer->is_beeping != 0U) && (p_buzzer->frame_count >= p_buzzer->pattern.on_frames))
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
    else if ((p_buzzer->is_beeping == 0U) && (p_buzzer->frame_count >= p_buzzer->pattern.off_frames))
    {
        start_beep(p_buzzer);
    }
}

void buzzer_init(void)
{
    McuIo_GpioInit(SMARTCAR_GPIO_BUZZER, MCUIO_GPIO_OUTPUT);
}

void Buzzer_Trigger(buzzer_event_t event)
{
    if ((event != BUZZER_EVENT_NONE) && (s_buzzer.active == 0U))
    {
        fire_pattern(&s_buzzer, event);
    }
}

void Buzzer_Tick(void)
{
    step_pattern(&s_buzzer);
}

uint8_t Buzzer_IsBusy(void)
{
    return s_buzzer.active;
}
