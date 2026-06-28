/*
 * control.c
 *
 * Control decision + actuation layer.
 * Extracted from image.c to separate vision processing from control logic.
 */

#include "control.h"
#include "zf_common_headfile.h"
#include "config.h"

void Control_Update(void)
{
    if(lost_count > LOST_LINE_THRESHOLD)
    {
        return;
    }
    servo_pid_output = servo_pid_contorl(&servo_pid, 0, calculate_error);
    left_motor_pid_output = motor_pid_control(&left_motor_pid, 0, left_encoder_speed);
    right_motor_pid_output = motor_pid_control(&right_motor_pid, 0, right_encoder_speed);
}

void Actuator_Apply(void)
{
    if(lost_count > LOST_LINE_THRESHOLD)
    {
        pwm_set_duty(ATOM1_CH1_P33_9, SERVO_CENTER_DUTY);
        return;
    }
    pwm_set_duty(ATOM1_CH1_P33_9, SERVO_CENTER_DUTY + (int)servo_pid_output);
    motor_setspeed_left((int32)left_motor_pid_output);
    motor_setspeed_right((int32)right_motor_pid_output);
}
