/**
 * @file smartcar_board.c
 * @brief Smart car board startup。
 */

#include "smartcar_board.h"
#include "platform/board/board_ops_if.h"
#include "system/board/smartcar_board_resources.h"
#include "platform/interface/actuator_if.h"
#include "platform/interface/feedback_if.h"
#include "platform/interface/track_display_if.h"
#include "display.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"

/* ── 前置声明 ──────────────────────────────────────── */
static void board_buzzer_notify(uint8_t element);

/* ── Board 本地 ops 表 ─────────────────────────────── */
static const actuator_ops_t s_actuator_ops = {
    .set_servo = Servo_SetAngle,
    .set_motor_left = Motor_SetLeft,
    .set_motor_right = Motor_SetRight,
};
static const feedback_ops_t s_feedback_ops = {
    .notify_element = board_buzzer_notify,
    .is_busy = Buzzer_IsBusy,
    .tick = Buzzer_Tick,
};
static const track_display_ops_t s_track_ops = {
    .draw_lines = Display_DrawTrackLines,
};

static void board_buzzer_notify(uint8_t element) {
    if (element == 1U) Buzzer_Trigger(BUZZER_EVENT_RING);
    else if (element == 2U) Buzzer_Trigger(BUZZER_EVENT_CROSSROAD);
}

void Board_BindOps(const target_board_ops_t *p_ops)
{
    if (p_ops == 0) { return; }
    McuIo_GpioRegister(p_ops->gpio);
    McuIo_PwmRegister(p_ops->pwm);
    McuIo_UartRegister(p_ops->uart);
    McuIo_EncoderRegister(p_ops->encoder);
    McuIo_PitRegister(p_ops->pit);
    Device_CameraRegister(p_ops->camera);
    Device_DisplayRegister(p_ops->display);
    Device_ImuRegister(p_ops->imu);
    Device_WirelessRegister(p_ops->wireless);
    Device_KeyRegister(p_ops->key);
    Actuator_Register(&s_actuator_ops);
    Feedback_Register(&s_feedback_ops);
    TrackDisplay_Register(&s_track_ops);
}

void SmartcarBoard_InitDevices(void)
{
    McuIo_UartInit(SMARTCAR_UART_CAMERA, 115200U);
    McuIo_UartInit(SMARTCAR_UART_BT, 115200U);
    McuIo_EncoderInit(SMARTCAR_ENCODER_LEFT);
    McuIo_EncoderInit(SMARTCAR_ENCODER_RIGHT);
    Motor_Init();
    Servo_Init();
    Device_DisplayInit();
    Device_CameraInit();
    Input_Init();
    buzzer_init();
    Device_ImuInit();
    Device_WirelessInit();
}

void SmartcarBoard_StartPeriodicIrq(void)
{
    McuIo_PitInit(SMARTCAR_PIT_ENCODER_SAMPLE, 10U);
    McuIo_PitInit(SMARTCAR_PIT_GYRO_TICK, 10U);
}
