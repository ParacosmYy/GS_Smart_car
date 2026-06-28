/*
 * buzzer.c
 */
#include "buzzer.h"
#include "config.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"
typedef struct { uint8_t beep_count, on_frames, off_frames; } buzzer_pattern_t;
static const buzzer_pattern_t s_patterns[] = {
    [BUZZER_EVENT_NONE]={0,0,0},[BUZZER_EVENT_RING]={3,BUZZER_BEEP_SHORT_F,BUZZER_GAP_F},
    [BUZZER_EVENT_CROSSROAD]={1,BUZZER_BEEP_LONG_F,0},
};
typedef struct { buzzer_pattern_t pattern; uint8_t remaining,frame_cnt,is_beeping,active; } buzzer_t;
static buzzer_t s_buzzer = {0};
static void start_beep(buzzer_t *b){McuIo_GpioHigh(SMARTCAR_GPIO_BUZZER);b->is_beeping=1;b->frame_cnt=0;}
static void enter_gap(buzzer_t *b) {McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);b->is_beeping=0;b->frame_cnt=0;b->remaining--;}
static void finish(buzzer_t *b)    {McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);b->active=0;b->is_beeping=0;}
static void fire(buzzer_t *b, buzzer_event_t e){b->pattern=s_patterns[e];b->remaining=b->pattern.beep_count;b->active=1;start_beep(b);}
static void step(buzzer_t *b){
    if(!b->active)return;b->frame_cnt++;
    if(b->is_beeping&&b->frame_cnt>=b->pattern.on_frames){
        if(b->remaining>1){enter_gap(b);}else{finish(b);}
    }else if(!b->is_beeping&&b->frame_cnt>=b->pattern.off_frames){start_beep(b);}
}
void buzzer_on(void){McuIo_GpioHigh(SMARTCAR_GPIO_BUZZER);}
void buzzer_off(void){McuIo_GpioLow(SMARTCAR_GPIO_BUZZER);}
void buzzer_init(void){McuIo_GpioInit(SMARTCAR_GPIO_BUZZER,MCUIO_GPIO_OUTPUT);}
void Buzzer_Trigger(buzzer_event_t e){if(e!=BUZZER_EVENT_NONE&&!s_buzzer.active)fire(&s_buzzer,e);}
void Buzzer_Tick(void){step(&s_buzzer);}
uint8_t Buzzer_IsBusy(void){return s_buzzer.active;}
