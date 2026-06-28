/**
 * @file actuator_dispatch.c
 * @brief 执行器 ops dispatch — Actuator_* API。
 */
#include "platform/interface/actuator_if.h"
static const actuator_ops_t *s_ops = 0;
void Actuator_Register(const actuator_ops_t *p) { s_ops = p; }
void Actuator_SetServo(int32_t o)    { if (s_ops && s_ops->set_servo) s_ops->set_servo(o); }
void Actuator_SetMotorLeft(int32_t s)  { if (s_ops && s_ops->set_motor_left) s_ops->set_motor_left(s); }
void Actuator_SetMotorRight(int32_t s) { if (s_ops && s_ops->set_motor_right) s_ops->set_motor_right(s); }
