/*
 * servo.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 *
 * @brief 舵机驱动 + 50Hz PWM 输出
 *        面向对象封装：全部运行时状态收敛到 servo_t 结构体
 *        内部函数以 servo_t* 为第一参数，公开 API 包装单实例
 */

#include "servo.h"
#include "config.h"

/* ===== 舵机对象（封装全部运行时状态）===== */
/* 单实例（全项目仅一个舵机） */
static servo_t s_servo = { 0 };

/* ===== 内部操作（均以 servo_t* 为第一参数）===== */

/**
 * @brief 将中位偏移量映射为占空比并写入硬件
 *        内部限幅到 [center - range, center + range]，防止打死方向
 */
static void servo_apply(servo_t *s, int32_t offset)
{
    int32_t lo     = (int32_t)s->center - (int32_t)s->range;
    int32_t hi     = (int32_t)s->center + (int32_t)s->range;
    int32_t target = (int32_t)s->center + offset;
    uint32_t duty;

    if (target < lo)
    {
        target = lo;
    }
    if (target > hi)
    {
        target = hi;
    }

    duty     = (uint32_t)target;
    s->duty  = duty;
    pal_pwm_set_duty(PAL_CH_SERVO, duty);
}

/* ===== 公开 API（包装单实例 s_servo）===== */

void Servo_Init(void)
{
    s_servo.center = SERVO_CENTER_DUTY;
    s_servo.range  = SERVO_RANGE;
    s_servo.duty   = SERVO_INIT_DUTY;

    pal_pwm_init(PAL_CH_SERVO, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}

void Servo_SetAngle(int32_t offset)
{
    servo_apply(&s_servo, offset);
}
