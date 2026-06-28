/*
 * pid.h
 *
 *  Created on: 2025Äê10ÔÂ19ÈÕ
 *      Author: Paracosm
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

typedef struct {
    float kp, ki, kd;
    float integral;
    float prev_err;
    float output;
} PosPID_t;

typedef struct {
    float kp, ki, kd;
    float prev_err, prev_prev_err;
    float output;
} IncPID_t;

void PosPID_Init(PosPID_t *pid, float kp, float ki, float kd);
void IncPID_Init(IncPID_t *pid, float kp, float ki, float kd);
float PosPID_Calc(PosPID_t *pid, float target, float feedback);
float IncPID_Calc(IncPID_t *pid, float target, float feedback);
float servo_pid_contorl(PosPID_t *steer_pid , float steer_target , float steer_feedback);
float motor_pid_control(IncPID_t *motor_pid, float speed_target, float speed_feedback);

extern float servo_pid_output ;
extern PosPID_t servo_pid ;
extern IncPID_t left_motor_pid ;
extern IncPID_t right_motor_pid ;

extern int left_encoder_speed;
extern int right_encoder_speed;

extern float left_motor_pid_output ;
extern float right_motor_pid_output ;

#endif /* CODE_PID_H_ */
