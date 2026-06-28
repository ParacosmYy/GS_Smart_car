#ifndef CODE_BUZZER_H_
#define CODE_BUZZER_H_
#include <stdint.h>
typedef enum { BUZZER_EVENT_NONE=0, BUZZER_EVENT_RING, BUZZER_EVENT_CROSSROAD } buzzer_event_t;
void buzzer_on(void);
void buzzer_off(void);
void Buzzer_Trigger(buzzer_event_t event);
void Buzzer_Tick(void);
uint8_t Buzzer_IsBusy(void);
void buzzer_init(void);
#endif
