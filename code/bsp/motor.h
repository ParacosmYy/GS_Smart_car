#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include <stdint.h>

void Motor_Init(void);
void Motor_SetLeft(int32_t speed);
void Motor_SetRight(int32_t speed);

#endif /* CODE_MOTOR_H_ */
