#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_
#include <stdint.h>
typedef struct { uint32_t duty, center, range; } servo_t;
void Servo_Init(void);
void Servo_SetAngle(int32_t offset);
#endif
