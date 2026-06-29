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
#include "platform/port_if.h"
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
 * @brief 绘制视觉边线和中线。
 *
 * Steps:
 *   1. 读取视觉调试快照。
 *   2. 通过 TrackDisplay 端口绘制三条赛道线。
 *
 * @return void。
 */
void DebugDisplayService_DrawVisionLines(void)
{
    vision_debug_snapshot_t vision_snapshot = {0};
    Vision_GetDebugSnapshot(&vision_snapshot);
    Display_DrawTrackLines(vision_snapshot.p_left_line,
                           vision_snapshot.p_right_line,
                           vision_snapshot.p_mid_line,
                           vision_snapshot.line_count);
}

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
    control_output_t control_output = {0.0f, 0.0f, 0.0f};
    (void)events;

    Vision_GetDebugSnapshot(&vision_snapshot);
    Control_GetOutputSnapshot(&control_output);

    Device_DisplayGray(DEBUG_DISPLAY_IMAGE_X, DEBUG_DISPLAY_IMAGE_Y,
                       vision_snapshot.p_binary_zip[0],
                       vision_snapshot.image_width, vision_snapshot.image_height,
                       vision_snapshot.image_width, vision_snapshot.image_height, 0);

    Device_DisplayStr(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_LEFT_ENCODER_Y, "left:");
    Device_DisplayInt(DEBUG_DISPLAY_ENCODER_VALUE_X, DEBUG_DISPLAY_LEFT_ENCODER_Y,
                      SensorService_GetLeftEncoderSpeed(), DEBUG_DISPLAY_ENCODER_DIGITS);

    Device_DisplayStr(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_RIGHT_ENCODER_Y, "right:");
    Device_DisplayInt(DEBUG_DISPLAY_ENCODER_VALUE_X, DEBUG_DISPLAY_RIGHT_ENCODER_Y,
                      SensorService_GetRightEncoderSpeed(), DEBUG_DISPLAY_ENCODER_DIGITS);

    Device_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_LEFT_PID_Y, "l_spd:");
    Device_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_RIGHT_PID_Y, "r_spd:");
    Device_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_LEFT_PID_Y,
                      (int32_t)control_output.left_motor, DEBUG_DISPLAY_PID_DIGITS);
    Device_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_RIGHT_PID_Y,
                      (int32_t)control_output.right_motor, DEBUG_DISPLAY_PID_DIGITS);

    Device_DisplayStr(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_ERROR_Y, "err:");
    Device_DisplayInt(DEBUG_DISPLAY_PID_VALUE_X, DEBUG_DISPLAY_ERROR_Y,
                      vision_snapshot.calculate_error, DEBUG_DISPLAY_ERROR_DIGITS);

    Device_DisplayStr(DEBUG_DISPLAY_STATUS_LABEL_X, DEBUG_DISPLAY_GYRO_OVERFLOW_Y, "gof:");
    Device_DisplayInt(DEBUG_DISPLAY_STATUS_VALUE_X, DEBUG_DISPLAY_GYRO_OVERFLOW_Y,
                      event_is_gyro_10ms_overflow_latched(), DEBUG_DISPLAY_STATUS_DIGITS);
}
