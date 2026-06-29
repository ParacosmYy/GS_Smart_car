#ifndef SERVICE_PORT_IF_H_
#define SERVICE_PORT_IF_H_

/**
 * @file service_port_if.h
 * @brief Service 到本车板级动作的端口接口。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Actuator_SetServo(int32_t offset);
void Actuator_SetMotorLeft(int32_t speed);
void Actuator_SetMotorRight(int32_t speed);

void Feedback_NotifyElement(uint8_t element);
uint8_t Feedback_IsBusy(void);
void Feedback_Tick(void);

void TrackDisplay_DrawLines(const uint8_t *p_left,
                            const uint8_t *p_right,
                            const uint8_t *p_mid,
                            uint8_t count);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_PORT_IF_H_ */
