/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file debug_display.c
 * @brief 调试显示服务实现。
 * @author GS_Mark
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

#include <stdint.h>
#include "debug_display.h"
#include "control.h"
#include "display.h"
#include "event.h"
#include "hal/hal.h"
#include "sensor.h"
#include "vision.h"

#define DEBUG_DISPLAY_IMAGE_X             0
#define DEBUG_DISPLAY_IMAGE_Y             0
#define DEBUG_DISPLAY_ENCODER_LABEL_X     0
#define DEBUG_DISPLAY_ENCODER_VALUE_X     50
#define DEBUG_DISPLAY_LEFT_ENCODER_Y      80
#define DEBUG_DISPLAY_RIGHT_ENCODER_Y     60
#define DEBUG_DISPLAY_PID_LABEL_X         0
#define DEBUG_DISPLAY_PID_VALUE_X         50
#define DEBUG_DISPLAY_LEFT_PID_Y          100
#define DEBUG_DISPLAY_RIGHT_PID_Y         120
#define DEBUG_DISPLAY_ERROR_Y             140
#define DEBUG_DISPLAY_STATUS_LABEL_X      100
#define DEBUG_DISPLAY_STATUS_VALUE_X      135
#define DEBUG_DISPLAY_GYRO_OVERFLOW_Y     60
#define DEBUG_DISPLAY_ENCODER_DIGITS      4
#define DEBUG_DISPLAY_PID_DIGITS          6
#define DEBUG_DISPLAY_ERROR_DIGITS        4
#define DEBUG_DISPLAY_STATUS_DIGITS       1

/**
 * @brief 刷新调试显示信息。
 *
 * Steps:
 *   1. 获取视觉调试快照和控制输出快照。
 *   2. 显示压缩二值图像、编码器速度、PID 输出和视觉误差。
 *
 * @param[in] events 本轮调度事件掩码，当前仅保留给扩展显示使用。
 * @return void。
 */
void DebugDisplayService_Update(uint32_t events)
{
    vision_debug_snapshot_t vision_snapshot = {0};
    sensor_service_snapshot_t sensor_snapshot = {0};
    control_output_t control_output = {0.0f, 0.0f, 0.0f};
    (void)events;

    Vision_GetDebugSnapshot(&vision_snapshot);
    SensorService_GetSnapshot(&sensor_snapshot);
    Control_GetOutputSnapshot(&control_output);

    SmartcarHal_DisplayGray(DEBUG_DISPLAY_IMAGE_X, DEBUG_DISPLAY_IMAGE_Y,
                            vision_snapshot.p_binary_zip[0],
                            vision_snapshot.image_width, vision_snapshot.image_height,
                            vision_snapshot.image_width, vision_snapshot.image_height, 0);
    Display_DrawTrackLines(vision_snapshot.p_left_line,
                           vision_snapshot.p_right_line,
                           vision_snapshot.p_mid_line,
                           vision_snapshot.line_count);

    SmartcarHal_DisplayStr(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_LEFT_ENCODER_Y, "left:");
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_ENCODER_VALUE_X, DEBUG_DISPLAY_LEFT_ENCODER_Y,
                           sensor_snapshot.left_encoder_speed, DEBUG_DISPLAY_ENCODER_DIGITS);

    SmartcarHal_DisplayStr(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_RIGHT_ENCODER_Y, "right:");
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_ENCODER_VALUE_X, DEBUG_DISPLAY_RIGHT_ENCODER_Y,
                           sensor_snapshot.right_encoder_speed, DEBUG_DISPLAY_ENCODER_DIGITS);

    SmartcarHal_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_LEFT_PID_Y, "l_spd:");
    SmartcarHal_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_RIGHT_PID_Y, "r_spd:");
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_LEFT_PID_Y,
                           (int32_t)control_output.left_motor, DEBUG_DISPLAY_PID_DIGITS);
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_RIGHT_PID_Y,
                           (int32_t)control_output.right_motor, DEBUG_DISPLAY_PID_DIGITS);

    SmartcarHal_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_ERROR_Y, "err:");
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_ERROR_Y,
                           vision_snapshot.calculate_error, DEBUG_DISPLAY_ERROR_DIGITS);

    SmartcarHal_DisplayStr(DEBUG_DISPLAY_STATUS_LABEL_X, DEBUG_DISPLAY_GYRO_OVERFLOW_Y, "gof:");
    SmartcarHal_DisplayInt(DEBUG_DISPLAY_STATUS_VALUE_X, DEBUG_DISPLAY_GYRO_OVERFLOW_Y,
                           Event_IsGyro10msOverflowLatched(), DEBUG_DISPLAY_STATUS_DIGITS);
}
