/**
 * @file motor.c
 * @brief 双直流电机 PWM BSP。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本模块保存左右电机输出快照与限幅参数，向上只暴露左右速度设置接口。
 */
#include "motor.h"

#include "config.h"
#include "smartcar_board_resources.h"
#include "platform/interface/mcu_io_if.h"

typedef struct
{
    int32_t speed_l;
    int32_t speed_r;
    int32_t clamp_l;
    int32_t clamp_r;
} motor_t;

static motor_t s_motor = {0};

/**
 * @brief 对电机速度命令做对称限幅。
 *
 * Steps:
 *   1. 将正向速度限制在 clamp 以内。
 *   2. 将反向速度限制在 -clamp 以内。
 *
 * @param[in] speed 输入速度命令。
 * @param[in] clamp 速度绝对值上限。
 * @return 限幅后的速度命令。
 */
static int32_t motor_clamp_speed(int32_t speed, int32_t clamp)
{
    int32_t clamped = speed;

    if (speed > clamp)
    {
        clamped = clamp;
    }
    else if (speed < -clamp)
    {
        clamped = -clamp;
    }

    return clamped;
}

/**
 * @brief 将左电机速度命令转换为正反转 PWM。
 *
 * Steps:
 *   1. 记录左电机输出快照。
 *   2. 正速度驱动正转通道，负速度驱动反转通道。
 *   3. 关闭未使用方向的 PWM 占空比。
 *
 * @param[in,out] p_motor 电机状态对象。
 * @param[in] speed 已限幅速度命令。
 * @return void。
 */
static void motor_apply_left(motor_t *p_motor, int32_t speed)
{
    p_motor->speed_l = speed;

    if (speed > 0)
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_FWD, (uint32_t)(speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_REV, 0U);
    }
    else
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_REV, (uint32_t)(-speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_L_FWD, 0U);
    }
}

/**
 * @brief 将右电机速度命令转换为正反转 PWM。
 *
 * Steps:
 *   1. 记录右电机输出快照。
 *   2. 正速度驱动正转通道，负速度驱动反转通道。
 *   3. 关闭未使用方向的 PWM 占空比。
 *
 * @param[in,out] p_motor 电机状态对象。
 * @param[in] speed 已限幅速度命令。
 * @return void。
 */
static void motor_apply_right(motor_t *p_motor, int32_t speed)
{
    p_motor->speed_r = speed;

    if (speed > 0)
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_FWD, (uint32_t)(speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_REV, 0U);
    }
    else
    {
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_REV, (uint32_t)(-speed * 100));
        McuIo_PwmSetDuty(SMARTCAR_PWM_MOTOR_R_FWD, 0U);
    }
}

/**
 * @brief 初始化左右电机 PWM 通道。
 *
 * Steps:
 *   1. 从配置表装载左右电机限幅。
 *   2. 初始化左右正反转四路 PWM，默认占空比为 0。
 *
 * @return void。
 */
void Motor_Init(void)
{
    s_motor.clamp_l = MOTOR_CLAMP_LEFT;
    s_motor.clamp_r = MOTOR_CLAMP_RIGHT;

    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_R_REV, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_R_FWD, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_L_REV, MOTOR_PWM_HZ, 0U);
    McuIo_PwmInit(SMARTCAR_PWM_MOTOR_L_FWD, MOTOR_PWM_HZ, 0U);
}

/**
 * @brief 设置左电机速度命令。
 *
 * Steps:
 *   1. 按左电机限幅裁剪输入速度。
 *   2. 应用到左电机正反转 PWM。
 *
 * @param[in] speed 左电机速度命令。
 * @return void。
 */
void Motor_SetLeft(int32_t speed)
{
    motor_apply_left(&s_motor, motor_clamp_speed(speed, s_motor.clamp_l));
}

/**
 * @brief 设置右电机速度命令。
 *
 * Steps:
 *   1. 按右电机限幅裁剪输入速度。
 *   2. 应用到右电机正反转 PWM。
 *
 * @param[in] speed 右电机速度命令。
 * @return void。
 */
void Motor_SetRight(int32_t speed)
{
    motor_apply_right(&s_motor, motor_clamp_speed(speed, s_motor.clamp_r));
}
