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
 *****************************************************************************/

//******************************** Includes *********************************//
#include <math.h>
#include <stdint.h>
#include "sensor.h"
#include "config.h"
#include "platform/interface/device_if.h"
#include "platform/system/encoder_sample.h"
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
    {{0.0f}, 0.0f, 0.0f, 0.0f, ((float)10 / 1000.0f), 0.0f, 0U}
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
static void SensorService_UpdateGyroOffset(sensor_gyro_context_t *p_gyro_ctx)
{
    if (p_gyro_ctx == 0) { return; }
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

static void SensorService_UpdateEncoderSpeed(sensor_encoder_context_t *p_encoder_ctx,
                                             int left_sum, int right_sum, int sample_count)
{
    if ((p_encoder_ctx == 0) || (sample_count <= 0)) { return; }
    p_encoder_ctx->left_speed = left_sum / sample_count;
    p_encoder_ctx->right_speed = right_sum / sample_count;
}

void SensorService_ProcessGyro10ms(void)
{
    sensor_gyro_context_t *p_gyro_ctx = &s_sensor_service_ctx.gyro;
    float z_angle_speed = 0.0f;

    Device_ImuRead();
    p_gyro_ctx->raw_z = Device_ImuZ();
    SensorService_UpdateGyroOffset(p_gyro_ctx);

    z_angle_speed = p_gyro_ctx->raw_z - p_gyro_ctx->z_offset;
    p_gyro_ctx->heading_angle += z_angle_speed * p_gyro_ctx->sample_period_s;
}

void SensorService_ProcessEncoder50ms(void)
{
    sensor_encoder_context_t *p_encoder_ctx = &s_sensor_service_ctx.encoder;
    int left_sum_snapshot = 0;
    int right_sum_snapshot = 0;
    int sample_count_snapshot = 0;

    EncoderSample_TakeSnapshot(&left_sum_snapshot, &right_sum_snapshot, &sample_count_snapshot);
    SensorService_UpdateEncoderSpeed(p_encoder_ctx,
                                     left_sum_snapshot,
                                     right_sum_snapshot,
                                     sample_count_snapshot);
}

int SensorService_GetLeftEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.left_speed;
}

int SensorService_GetRightEncoderSpeed(void)
{
    return s_sensor_service_ctx.encoder.right_speed;
}

float SensorService_GetHeadingAngle(void)
{
    return s_sensor_service_ctx.gyro.heading_angle;
}

void SensorService_ResetHeadingAngle(void)
{
    s_sensor_service_ctx.gyro.heading_angle = 0.0f;
}

void SensorService_SetGyroSamplePeriod(float sample_period_s)
{
    if (sample_period_s > 0.0f)
    {
        s_sensor_service_ctx.gyro.sample_period_s = sample_period_s;
    }
}
//******************************** Implement ********************************//
