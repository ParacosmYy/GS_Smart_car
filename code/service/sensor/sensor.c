/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file sensor.c
 *
 * @par dependencies
 * - sensor.h
 *
 * @author GS_Mark
 *
 * @brief Sensor service implementation.
 *
 * Processing flow:
 * The ISR adapter only publishes events and accumulates raw counts. This
 * service runs in the main loop to compensate gyro drift, integrate heading,
 * and average encoder speed, keeping algorithms out of ISR context.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include <math.h>
#include <stdint.h>
#include "sensor.h"
#include "config.h"
#include "platform.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define GYRO_IDLE_THRESHOLD  1.0f
#define GYRO_OFFSET_BUF_SIZE 32U
//******************************** Defines **********************************//

//******************************** Types ************************************//
typedef struct
{
    int left_speed;
    int right_speed;
} sensor_encoder_context_t;

typedef struct
{
    float   z_offset_buf[GYRO_OFFSET_BUF_SIZE];
    float   z_offset_sum;
    float   z_offset;
    float   heading_angle;
    float   sample_period_s;
    float   raw_z;
    uint8_t z_offset_idx;
} sensor_gyro_context_t;

typedef struct
{
    sensor_encoder_context_t encoder;
    sensor_gyro_context_t    gyro;
} sensor_service_context_t;
//******************************** Types ************************************//

//******************************** Variables ********************************//
static sensor_service_context_t s_sensor_service_ctx =
{
    {0, 0},
    {{0.0f}, 0.0f, 0.0f, 0.0f, ((float)PIT_PERIOD_MS / 1000.0f), 0.0f, 0U}
};
//******************************** Variables ********************************//

//******************************** Declaring ********************************//
static void SensorService_UpdateGyroOffset(sensor_gyro_context_t *p_gyro_ctx);
static void SensorService_UpdateEncoderSpeed(sensor_encoder_context_t *p_encoder_ctx,
                                             int left_sum,
                                             int right_sum,
                                             int sample_count);
//******************************** Declaring ********************************//

//******************************** Implement ********************************//
/**
 * @brief 更新陀螺仪零漂估计。
 *
 * 处理步骤：
 *  1. 仅在 Z 轴角速度接近静止时更新滑动窗口。
 *  2. 用窗口和计算当前零漂偏置。
 *
 * @param[in,out] p_gyro_ctx : 陀螺仪 Handler 上下文。
 *
 * @return void : 无返回值。
 *
 * */
static void SensorService_UpdateGyroOffset(sensor_gyro_context_t *p_gyro_ctx)
{
    if (p_gyro_ctx == 0)
    {
        return;
    }

    if (fabsf(p_gyro_ctx->raw_z) < GYRO_IDLE_THRESHOLD)
    {
        p_gyro_ctx->z_offset_sum -= p_gyro_ctx->z_offset_buf[p_gyro_ctx->z_offset_idx];
        p_gyro_ctx->z_offset_buf[p_gyro_ctx->z_offset_idx] = p_gyro_ctx->raw_z;
        p_gyro_ctx->z_offset_sum += p_gyro_ctx->raw_z;

        p_gyro_ctx->z_offset_idx++;
        if (p_gyro_ctx->z_offset_idx >= GYRO_OFFSET_BUF_SIZE)
        {
            p_gyro_ctx->z_offset_idx = 0U;
        }

        p_gyro_ctx->z_offset = p_gyro_ctx->z_offset_sum / GYRO_OFFSET_BUF_SIZE;
    }
}

/**
 * @brief 根据编码器累加快照更新平均速度。
 *
 * 处理步骤：
 *  1. 防御采样次数为 0 的异常快照。
 *  2. 将左右累加值转换为当前测速窗口平均速度。
 *
 * @param[in,out] p_encoder_ctx : 编码器 Handler 上下文。
 * @param[in]     left_sum      : 左编码器窗口累加值。
 * @param[in]     right_sum     : 右编码器窗口累加值。
 * @param[in]     sample_count  : 窗口内采样次数。
 *
 * @return void : 无返回值。
 *
 * */
static void SensorService_UpdateEncoderSpeed(sensor_encoder_context_t *p_encoder_ctx,
                                             int left_sum,
                                             int right_sum,
                                             int sample_count)
{
    if ((p_encoder_ctx == 0) || (sample_count <= 0))
    {
        return;
    }

    p_encoder_ctx->left_speed = left_sum / sample_count;
    p_encoder_ctx->right_speed = right_sum / sample_count;
}

/**
 * @brief 处理 10ms 陀螺仪采样事件。
 *
 * 处理步骤：
 *  1. 读取陀螺仪 Z 轴角速度。
 *  2. 静止时更新零漂滑动窗口。
 *  3. 用去零漂后的角速度积分得到航向角。
 *
 * @return void : 无返回值。
 *
 * */
void SensorService_ProcessGyro10ms(void)
{
    sensor_gyro_context_t *p_gyro_ctx = &s_sensor_service_ctx.gyro;
    float z_angle_speed = 0.0f;

    pal_gyro_read();
    p_gyro_ctx->raw_z = pal_gyro_z();
    SensorService_UpdateGyroOffset(p_gyro_ctx);

    z_angle_speed = p_gyro_ctx->raw_z - p_gyro_ctx->z_offset;
    p_gyro_ctx->heading_angle += z_angle_speed * p_gyro_ctx->sample_period_s;
}

/**
 * @brief 处理 50ms 编码器测速事件。
 *
 * 处理步骤：
 *  1. 从中断适配层获取并清零编码器累加快照。
 *  2. 按实际采样次数计算左右轮平均速度。
 *
 * @return void : 无返回值。
 *
 * */
void SensorService_ProcessEncoder50ms(void)
{
    sensor_encoder_context_t *p_encoder_ctx = &s_sensor_service_ctx.encoder;
    int left_sum_snapshot = 0;
    int right_sum_snapshot = 0;
    int sample_count_snapshot = 0;

    pal_encoder_take_snapshot(&left_sum_snapshot, &right_sum_snapshot, &sample_count_snapshot);

    SensorService_UpdateEncoderSpeed(p_encoder_ctx,
                                     left_sum_snapshot,
                                     right_sum_snapshot,
                                     sample_count_snapshot);
}

/**
 * @brief 获取左轮编码器速度。
 *
 * @return int : 左轮编码器平均速度。
 *
 * */
int SensorService_GetLeftEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.left_speed;
}

/**
 * @brief 获取右轮编码器速度。
 *
 * @return int : 右轮编码器平均速度。
 *
 * */
int SensorService_GetRightEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.right_speed;
}

/**
 * @brief 获取陀螺仪积分航向角。
 *
 * @return float : Z 轴累计角度，单位：度。
 *
 * */
float SensorService_GetHeadingAngle(void)
{
    return s_sensor_service_ctx.gyro.heading_angle;
}

/**
 * @brief 复位陀螺仪积分航向角。
 *
 * @return void : 无返回值。
 *
 * */
void SensorService_ResetHeadingAngle(void)
{
    s_sensor_service_ctx.gyro.heading_angle = 0.0f;
}

/**
 * @brief 设置陀螺仪积分采样周期。
 *
 * @param[in] sample_period_s : 采样周期，单位：秒。
 *
 * @return void : 无返回值。
 *
 * */
void SensorService_SetGyroSamplePeriod(float sample_period_s)
{
    if (sample_period_s > 0.0f)
    {
        s_sensor_service_ctx.gyro.sample_period_s = sample_period_s;
    }
}
//******************************** Implement ********************************//
