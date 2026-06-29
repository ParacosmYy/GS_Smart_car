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
#include "platform/port_if.h"

typedef struct
{
    int32_t speed_l;
    int32_t speed_r;
    int32_t clamp_l;
    int32_t clamp_r;
} motor_t;

#define MOTOR_PWM_DUTY_SCALE  100

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
 * @brief 将电机速度命令转换为正反转 PWM。
 *
 * Steps:
 *   1. 记录电机输出快照。
 *   2. 正速度驱动传入的正转通道，负速度驱动传入的反转通道。
 *   3. 关闭未使用方向的 PWM 占空比。
 *
 * @param[in,out] p_speed_snapshot 电机速度快照。
 * @param[in] speed 已限幅速度命令。
 * @param[in] fwd_channel 正转 PWM 通道。
 * @param[in] rev_channel 反转 PWM 通道。
 * @return void。
 */
static void motor_apply_pwm(int32_t *p_speed_snapshot,
                            int32_t speed,
                            mcuio_pwm_id_t fwd_channel,
                            mcuio_pwm_id_t rev_channel)
{
    uint32_t duty = 0U;

    if (p_speed_snapshot == 0)
    {
        return;
    }

    *p_speed_snapshot = speed;

    if (speed > 0)
    {
        duty = (uint32_t)(speed * MOTOR_PWM_DUTY_SCALE);
        McuIo_PwmSetDuty(fwd_channel, duty);
        McuIo_PwmSetDuty(rev_channel, 0U);
    }
    else
    {
        duty = (uint32_t)(-speed * MOTOR_PWM_DUTY_SCALE);
        McuIo_PwmSetDuty(rev_channel, duty);
        McuIo_PwmSetDuty(fwd_channel, 0U);
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
    motor_apply_pwm(&s_motor.speed_l,
                    motor_clamp_speed(speed, s_motor.clamp_l),
                    SMARTCAR_PWM_MOTOR_L_FWD,
                    SMARTCAR_PWM_MOTOR_L_REV);
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
    motor_apply_pwm(&s_motor.speed_r,
                    motor_clamp_speed(speed, s_motor.clamp_r),
                    SMARTCAR_PWM_MOTOR_R_FWD,
                    SMARTCAR_PWM_MOTOR_R_REV);
}
