#ifndef ACTUATOR_IF_H_
#define ACTUATOR_IF_H_

/* 执行器领域契约：Service → BSP 的 ops 边界。*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*set_servo)(int32_t offset);
    void (*set_motor_left)(int32_t speed);
    void (*set_motor_right)(int32_t speed);
} actuator_ops_t;

void Actuator_Register(const actuator_ops_t *p_ops);
void Actuator_SetServo(int32_t offset);
void Actuator_SetMotorLeft(int32_t speed);
void Actuator_SetMotorRight(int32_t speed);

#ifdef __cplusplus
}
#endif
#endif
