#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_
#include <stdint.h>
typedef struct { int32_t speed_l, speed_r, clamp_l, clamp_r; } motor_t;
void Motor_Init(void);
void Motor_SetLeft(int32_t speed);
void Motor_SetRight(int32_t speed);
#endif
