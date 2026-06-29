/*
 * motor.c
 */
#include "motor.h"

#include "config.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"

typedef struct
{
    int32_t speed_l;
    int32_t speed_r;
    int32_t clamp_l;
    int32_t clamp_r;
} motor_t;

static motor_t s_motor = {0};

static int32_t motor_clamp_speed(int32_t speed, int32_t clamp)
{
    int32_t clamped = speed;

    if (speed > clamp)
    {
        clamped = clamp;
    }
    else if (speed < -clamp)
    {
        clamped = -clamp;
    }

    return clamped;
}

static void motor_apply_left(motor_t *p_motor, int32_t speed)
{
    p_motor->speed_l = speed;

    if (speed > 0)
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_FWD, (uint32_t)(speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_REV, 0U);
    }
    else
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_REV, (uint32_t)(-speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_FWD, 0U);
    }
}

static void motor_apply_right(motor_t *p_motor, int32_t speed)
{
    p_motor->speed_r = speed;

    if (speed > 0)
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_FWD, (uint32_t)(speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_REV, 0U);
    }
    else
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_REV, (uint32_t)(-speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_FWD, 0U);
    }
}

void Motor_Init(void)
{
    s_motor.clamp_l = MOTOR_CLAMP_LEFT;
    s_motor.clamp_r = MOTOR_CLAMP_RIGHT;

    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_R_REV, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_R_FWD, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_L_REV, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_L_FWD, MOTOR_PWM_HZ, 0U);
}

void Motor_SetLeft(int32_t speed)
{
    motor_apply_left(&s_motor, motor_clamp_speed(speed, s_motor.clamp_l));
}

void Motor_SetRight(int32_t speed)
{
    motor_apply_right(&s_motor, motor_clamp_speed(speed, s_motor.clamp_r));
}
