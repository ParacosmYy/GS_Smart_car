#ifndef CODE_CONTROL_CONTROL_H_
#define CODE_CONTROL_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute PID outputs for servo and motors based on vision + encoder data.
 *
 * Reads: calculate_error (vision), left/right_encoder_speed (ISR).
 * Writes: servo_pid_output, left_motor_pid_output, right_motor_pid_output.
 * No-op when lost_count exceeds LOST_LINE_THRESHOLD.
 */
void Control_Update(void);

/**
 * @brief Apply computed PID outputs to physical actuators (servo PWM + motor speed).
 *
 * When track is lost: servo set to center, motors keep last value.
 * When tracking: servo driven by PID, motors driven by PID.
 */
void Actuator_Apply(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_CONTROL_CONTROL_H_ */
