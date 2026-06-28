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
#ifndef SENSOR_H_
#define SENSOR_H_

/**
 * @brief 处理 10ms 陀螺仪采样事件
 */
void SensorService_ProcessGyro10ms(void);

/**
 * @brief 处理 50ms 编码器测速事件
 */
void SensorService_ProcessEncoder50ms(void);

/**
 * @brief 获取左轮编码器速度
 */
int SensorService_GetLeftEncoderSpeed(void);

/**
 * @brief 获取右轮编码器速度
 */
int SensorService_GetRightEncoderSpeed(void);

/**
 * @brief 获取陀螺仪积分航向角
 */
float SensorService_GetHeadingAngle(void);

/**
 * @brief 复位陀螺仪积分航向角
 */
void SensorService_ResetHeadingAngle(void);

/**
 * @brief 设置陀螺仪积分采样周期
 */
void SensorService_SetGyroSamplePeriod(float sample_period_s);

#endif /* SENSOR_H_ */
