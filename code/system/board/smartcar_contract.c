#include "buzzer.h"
#include "display.h"
#include "motor.h"
#include "platform/interface/service_port_if.h"
#include "servo.h"

void Actuator_SetServo(int32_t offset)
{
    Servo_SetAngle(offset);
}

void Actuator_SetMotorLeft(int32_t speed)
{
    Motor_SetLeft(speed);
}

void Actuator_SetMotorRight(int32_t speed)
{
    Motor_SetRight(speed);
}

void Feedback_NotifyElement(uint8_t element)
{
    if (element == 1U)
    {
        Buzzer_Trigger(BUZZER_EVENT_RING);
    }
    else if (element == 2U)
    {
        Buzzer_Trigger(BUZZER_EVENT_CROSSROAD);
    }
}

uint8_t Feedback_IsBusy(void)
{
    return Buzzer_IsBusy();
}

void Feedback_Tick(void)
{
    Buzzer_Tick();
}

void TrackDisplay_DrawLines(const uint8_t *p_left,
                            const uint8_t *p_right,
                            const uint8_t *p_mid,
                            uint8_t count)
{
    Display_DrawTrackLines(p_left, p_right, p_mid, count);
}
