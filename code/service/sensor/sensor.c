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
 * @version V1.0 2026-06-29
 *
 * @par 设计说明
 * Sensor service 将 IMU 与编码器采样转换为控制层可读的速度和航向快照。
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "sensor.h"
#include "config.h"
#include "platform/port_if.h"
#include "platform/sensor_hal.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define GYRO_IDLE_THRESHOLD  1.0f
#define GYRO_MS_PER_SECOND   1000.0f
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
    uint32_t                 version;
} sensor_service_context_t;
//******************************** Types ************************************//

//******************************** Variables ********************************//
static sensor_service_context_t s_sensor_service_ctx =
{
    {0, 0},
    {{0.0f}, 0.0f, 0.0f, 0.0f, ((float)PIT_PERIOD_MS / GYRO_MS_PER_SECOND), 0.0f, 0U},
    0U
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
 * @brief 更新陀螺仪静止零偏估计。
 *
 * Steps:
 *   1. 校验陀螺仪上下文指针。
 *   2. 仅在角速度接近静止阈值时更新滑动窗口。
 *   3. 用窗口平均值刷新 Z 轴零偏。
 *
 * @param[in,out] p_gyro_ctx 陀螺仪上下文。
 * @return void。
 */
static void SensorService_UpdateGyroOffset(sensor_gyro_context_t *p_gyro_ctx)
{
    if (p_gyro_ctx == NULL)
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
 * @brief 根据编码器窗口累加值更新平均速度。
 *
 * Steps:
 *   1. 校验编码器上下文和采样次数。
 *   2. 用左右累加值除以有效采样次数，得到窗口平均速度。
 *
 * @param[in,out] p_encoder_ctx 编码器上下文。
 * @param[in] left_sum 左编码器计数累加值。
 * @param[in] right_sum 右编码器计数累加值。
 * @param[in] sample_count 采样次数。
 * @return void。
 */
static void SensorService_UpdateEncoderSpeed(sensor_encoder_context_t *p_encoder_ctx,
                                             int left_sum, int right_sum, int sample_count)
{
    if ((p_encoder_ctx == NULL) || (sample_count <= 0))
    {
        return;
    }

    p_encoder_ctx->left_speed = left_sum / sample_count;
    p_encoder_ctx->right_speed = right_sum / sample_count;
}

/**
 * @brief 处理 10ms 陀螺仪采样。
 *
 * Steps:
 *   1. 读取 IMU 当前 Z 轴角速度。
 *   2. 更新静止零偏估计。
 *   3. 扣除零偏后积分得到航向角。
 *
 * @return void。
 */
void SensorService_ProcessGyro10ms(void)
{
    sensor_gyro_context_t *p_gyro_ctx = &s_sensor_service_ctx.gyro;
    float z_angle_speed = 0.0f;

    Device_ImuRead();
    p_gyro_ctx->raw_z = Device_ImuZ();
    SensorService_UpdateGyroOffset(p_gyro_ctx);

    z_angle_speed = p_gyro_ctx->raw_z - p_gyro_ctx->z_offset;
    p_gyro_ctx->heading_angle += z_angle_speed * p_gyro_ctx->sample_period_s;
    /* Bump after service state changes; snapshots copy this lightweight stamp. */
    s_sensor_service_ctx.version++;
}

/**
 * @brief 获取 Z 轴航向角积分值。
 *
 * @return 当前航向角积分值。
 */
float SensorService_GetHeadingAngle(void)
{
    return s_sensor_service_ctx.gyro.heading_angle;
}

/**
 * @brief 获取传感器服务快照。
 *
 * Steps:
 *   1. 校验调用方快照指针。
 *   2. 一次性复制航向角和左右编码器速度，减少控制层散读。
 *
 * @param[out] p_snapshot 传感器服务快照。
 * @return void。
 */
void SensorService_GetSnapshot(sensor_service_snapshot_t *p_snapshot)
{
    if (p_snapshot == NULL)
    {
        return;
    }

    p_snapshot->heading_angle = s_sensor_service_ctx.gyro.heading_angle;
    p_snapshot->left_encoder_speed = s_sensor_service_ctx.encoder.left_speed;
    p_snapshot->right_encoder_speed = s_sensor_service_ctx.encoder.right_speed;
    p_snapshot->version = s_sensor_service_ctx.version;
}

/**
 * @brief 处理 50ms 编码器采样窗口。
 *
 * Steps:
 *   1. 从 ISR adapter 获取编码器窗口快照。
 *   2. 更新左右轮平均速度。
 *
 * @return void。
 */
void SensorService_ProcessEncoder50ms(void)
{
    sensor_encoder_context_t *p_encoder_ctx = &s_sensor_service_ctx.encoder;
    int left_sum_snapshot = 0;
    int right_sum_snapshot = 0;
    int sample_count_snapshot = 0;

    SensorHal_EncoderTakeSnapshot(&left_sum_snapshot, &right_sum_snapshot, &sample_count_snapshot);
    SensorService_UpdateEncoderSpeed(p_encoder_ctx,
                                     left_sum_snapshot,
                                     right_sum_snapshot,
                                     sample_count_snapshot);
    if (sample_count_snapshot > 0)
    {
        s_sensor_service_ctx.version++;
    }
}

/**
 * @brief 获取左轮编码器速度。
 *
 * @return 左轮窗口平均速度。
 */
int SensorService_GetLeftEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.left_speed;
}

/**
 * @brief 获取右轮编码器速度。
 *
 * @return 右轮窗口平均速度。
 */
int SensorService_GetRightEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.right_speed;
}

//******************************** Implement ********************************//
