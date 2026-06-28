/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file sensor.c
 *
 * @par 依赖
 * - sensor.h
 *
 * @author GS_Mark
 *
 * @brief 传感器服务实现。
 *
 * 处理流程：
 * 中断适配层只负责发布事件和累加原始计数；本服务在主循环中完成
 * 陀螺仪零漂补偿、角度积分和编码器速度平均，避免在 ISR 中执行算法。
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

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
void Sensor_ProcessEncoder50ms(void)
{
    int left_sum_snapshot = 0;
    int right_sum_snapshot = 0;
    int sample_count_snapshot = 0;
    IsrAdapter_TakeEncoderSnapshot(&left_sum_snapshot, &right_sum_snapshot, &sample_count_snapshot);

    s_left_encoder_speed  = left_sum_snapshot / sample_count_snapshot;
    s_right_encoder_speed = right_sum_snapshot / sample_count_snapshot;
}

/**
 * @brief 获取左轮编码器速度。
 *
 * @return int : 左轮编码器平均速度。
 *
 * */
int Sensor_GetLeftEncoderSpeed(void)
{
    return s_left_encoder_speed;
}

/**
 * @brief 获取右轮编码器速度。
 *
 * @return int : 右轮编码器平均速度。
 *
 * */
int Sensor_GetRightEncoderSpeed(void)
{
    return s_right_encoder_speed;
}
