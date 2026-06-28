#include <math.h>
#include <stdint.h>
#include "sensor.h"
#include "data.h"
#include "isr_adapter.h"
#include "platform.h"

#define GYRO_IDLE_THRESHOLD  1.0f

static int s_left_encoder_speed = 0;
static int s_right_encoder_speed = 0;

static float   s_gyro_z_offset_buf[GYRO_OFFSET_BUF_SIZE] = {0.0f};
static uint8_t s_gyro_z_offset_idx = 0;
static float   s_gyro_z_offset_sum = 0.0f;
static float   s_gyro_z_offset     = 0.0f;
static float   s_gyro_raw_z        = 0.0f;

void Sensor_ProcessGyro10ms(void)
{
    float z_angle_speed = 0.0f;

    pal_gyro_read();
    s_gyro_raw_z = pal_gyro_z();

    if (fabsf(s_gyro_raw_z) < GYRO_IDLE_THRESHOLD)
    {
        s_gyro_z_offset_sum -= s_gyro_z_offset_buf[s_gyro_z_offset_idx];
        s_gyro_z_offset_buf[s_gyro_z_offset_idx] = s_gyro_raw_z;
        s_gyro_z_offset_sum += s_gyro_raw_z;

        s_gyro_z_offset_idx++;
        if (s_gyro_z_offset_idx >= GYRO_OFFSET_BUF_SIZE)
        {
            s_gyro_z_offset_idx = 0;
        }

        s_gyro_z_offset = s_gyro_z_offset_sum / GYRO_OFFSET_BUF_SIZE;
    }

    z_angle_speed = s_gyro_raw_z - s_gyro_z_offset;
    z_angle += z_angle_speed * dt;
}

void Sensor_ProcessEncoder50ms(void)
{
    int left_sum_snapshot = 0;
    int right_sum_snapshot = 0;
    int sample_count_snapshot = 0;
    IsrAdapter_TakeEncoderSnapshot(&left_sum_snapshot, &right_sum_snapshot, &sample_count_snapshot);

    s_left_encoder_speed  = left_sum_snapshot / sample_count_snapshot;
    s_right_encoder_speed = right_sum_snapshot / sample_count_snapshot;
}

int Sensor_GetLeftEncoderSpeed(void)
{
    return s_left_encoder_speed;
}

int Sensor_GetRightEncoderSpeed(void)
{
    return s_right_encoder_speed;
}
