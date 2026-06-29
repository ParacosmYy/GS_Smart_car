/*
 * servo.c
 */
#include "servo.h"

#include "config.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"

typedef struct
{
    uint32_t duty;
    uint32_t center;
    uint32_t range;
} servo_t;

static servo_t s_servo = {0};

static void servo_apply(servo_t *p_servo, int32_t offset)
{
    int32_t min_duty = (int32_t)p_servo->center - (int32_t)p_servo->range;
    int32_t max_duty = (int32_t)p_servo->center + (int32_t)p_servo->range;
    int32_t target = (int32_t)p_servo->center + offset;

    if (target < min_duty)
    {
        target = min_duty;
    }
    else if (target > max_duty)
    {
        target = max_duty;
    }

    p_servo->duty = (uint32_t)target;
    McuIo_PwmSetDuty(SMARTCAR_PWM_SERVO, (uint32_t)target);
}

void Servo_Init(void)
{
    s_servo.center = SERVO_CENTER_DUTY;
    s_servo.range = SERVO_RANGE;
    s_servo.duty = SERVO_INIT_DUTY;

    McuIo_PwmInit(SMARTCAR_PWM_SERVO, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}

void Servo_SetAngle(int32_t offset)
{
    servo_apply(&s_servo, offset);
}
