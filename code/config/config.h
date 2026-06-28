#ifndef CODE_CONFIG_CONFIG_H_
#define CODE_CONFIG_CONFIG_H_

/*===========================================================================*/
/*  Centralized tunable parameters for GS_Smart_car                          */
/*  All magic numbers that may need adjustment live here.                    */
/*===========================================================================*/

/* ===== Servo ===== */
#define SERVO_PWM_HZ            50      /* PWM frequency for servo            */
#define SERVO_CENTER_DUTY       678     /* Center position duty               */
#define SERVO_INIT_DUTY         692     /* Initial duty at startup            */
#define SERVO_RANGE             63      /* Max deviation from center          */

/* ===== Motor ===== */
#define MOTOR_PWM_HZ            20000   /* PWM frequency for motor drivers    */
#define MOTOR_CLAMP_LEFT        15      /* Left motor speed clamp (±)         */
#define MOTOR_CLAMP_RIGHT       100     /* Right motor speed clamp (±)        */

/* ===== PID Gains ===== */
#define SERVO_PID_KP            3.0f
#define SERVO_PID_KI            0.0f
#define SERVO_PID_KD            1.2f

#define MOTOR_PID_KP            0.3f
#define MOTOR_PID_KI            0.0f
#define MOTOR_PID_KD            0.0f

/* ===== Vision ===== */
#define IMAGE_MID_COL           47      /* Center column for error reference  */
#define LOST_LINE_THRESHOLD     10      /* Lost-line count to stop control    */
#define LOST_MIDLINE_GAP        40      /* Midline jump threshold for loss    */

/* ===== Periodic Interrupt ===== */
#define PIT_PERIOD_MS           10      /* CCU60 channel period in ms         */

#endif /* CODE_CONFIG_CONFIG_H_ */
