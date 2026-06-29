/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file sensor.h
 *
 * @par dependencies
 * - stdint.h
 *
 * @author GS_Mark
 *
 * @brief Sensor service interface.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef CODE_SERVICE_SENSOR_SENSOR_H_
#define CODE_SERVICE_SENSOR_SENSOR_H_

#include <stdint.h>

typedef struct
{
    float heading_angle;
    int left_encoder_speed;
    int right_encoder_speed;
    uint32_t version; /* Monotonic service stamp for snapshot comparisons. */
} sensor_service_snapshot_t;

/**
 * @brief 处理 10ms 陀螺仪采样事件
 */
void SensorService_ProcessGyro10ms(void);

/**
 * @brief 处理 50ms 编码器测速事件
 */
void SensorService_ProcessEncoder50ms(void);

/**
 * @brief 获取 Z 轴航向角积分值
 */
float SensorService_GetHeadingAngle(void);

/**
 * @brief 获取传感器服务快照
 */
void SensorService_GetSnapshot(sensor_service_snapshot_t *p_snapshot);

#endif /* CODE_SERVICE_SENSOR_SENSOR_H_ */
