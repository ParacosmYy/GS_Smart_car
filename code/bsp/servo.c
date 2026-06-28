/*
 * servo.c
 */
#include "servo.h"
#include "config.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"

static servo_t s_servo = { 0 };
static void servo_apply(servo_t *s, int32_t offset) {
    int32_t lo = (int32_t)s->center - (int32_t)s->range, hi = (int32_t)s->center + (int32_t)s->range, t = (int32_t)s->center + offset;
    if (t < lo) t = lo; if (t > hi) t = hi;
    s->duty = (uint32_t)t; McuIo_PwmSetDuty(SMARTCAR_PWM_SERVO, (uint32_t)t);
}
void Servo_Init(void) {
    s_servo.center = SERVO_CENTER_DUTY; s_servo.range = SERVO_RANGE; s_servo.duty = SERVO_INIT_DUTY;
    McuIo_PwmInit(SMARTCAR_PWM_SERVO, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}
void Servo_SetAngle(int32_t offset) { servo_apply(&s_servo, offset); }
