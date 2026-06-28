#ifndef CODE_INPUT_H_
#define CODE_INPUT_H_
#include <stdint.h>
typedef enum { IMAGE_VALUE=0, SERVO_VALUE, MOTOR_VALUE, CAR_RUN_VALUE } state_machine_enum;
extern state_machine_enum state;
typedef struct { uint8_t key_level[4], key_last_level[4], key_pressed[4], dip_switch[4]; } input_t;
void Input_Init(void);
void Input_Scan(void);
uint8_t Input_GetPressedKey(void);
#endif
