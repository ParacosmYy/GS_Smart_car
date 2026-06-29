/**
 * @file servo.c
 * @brief 转向舵机 PWM BSP。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本模块将控制层输出的舵机偏移量限制在机械安全范围内，再写入 PWM 占空比。
 */
#include "servo.h"

#include "config.h"
#include "smartcar_board_resources.h"
#include "platform/port_if.h"

#define SERVO_SAFE_MIN_DUTY  0LL
#define SERVO_SAFE_MAX_DUTY  ((int64_t)UINT32_MAX)

typedef struct
{
    uint32_t duty;
    uint32_t center;
    uint32_t range;
} servo_t;

static servo_t s_servo = {0};

/**
 * @brief 应用舵机偏移并执行机械限幅。
 *
 * Steps:
 *   1. 根据中位和允许范围计算占空比上下界。
 *   2. 将占空比上下界约束到 uint32_t 可表达范围。
 *   3. 将输入偏移叠加到中位。
 *   4. 限幅后写入 PWM。
 *
 * @param[in,out] p_servo 舵机状态对象。
 * @param[in] offset 相对中位的占空比偏移。
 * @return void。
 */
static void servo_apply(servo_t *p_servo, int32_t offset)
{
    int64_t min_duty = (int64_t)p_servo->center - (int64_t)p_servo->range;
    int64_t max_duty = (int64_t)p_servo->center + (int64_t)p_servo->range;
    int64_t target = (int64_t)p_servo->center + (int64_t)offset;

    if (min_duty < SERVO_SAFE_MIN_DUTY)
    {
        min_duty = SERVO_SAFE_MIN_DUTY;
    }

    if (max_duty > SERVO_SAFE_MAX_DUTY)
    {
        max_duty = SERVO_SAFE_MAX_DUTY;
    }

    if (max_duty < min_duty)
    {
        max_duty = min_duty;
    }

    if (target < min_duty)
    {
        target = min_duty;
    }
    else if (target > max_duty)
    {
        target = max_duty;
    }

    p_servo->duty = (uint32_t)target;
    McuIo_PwmSetDuty(SMARTCAR_PWM_SERVO, p_servo->duty);
}

/**
 * @brief 初始化舵机 PWM。
 *
 * Steps:
 *   1. 装载中位、初始占空比和允许范围。
 *   2. 初始化舵机 PWM 输出。
 *
 * @return void。
 */
void Servo_Init(void)
{
    s_servo.center = SERVO_CENTER_DUTY;
    s_servo.range = SERVO_RANGE;
    s_servo.duty = SERVO_INIT_DUTY;

    McuIo_PwmInit(SMARTCAR_PWM_SERVO, SERVO_PWM_HZ, SERVO_INIT_DUTY);
}

/**
 * @brief 设置舵机偏移。
 *
 * Steps:
 *   1. 将控制层输出传给内部限幅与 PWM 写入函数。
 *
 * @param[in] offset 相对中位的占空比偏移。
 * @return void。
 */
void Servo_SetAngle(int32_t offset)
{
    servo_apply(&s_servo, offset);
}
