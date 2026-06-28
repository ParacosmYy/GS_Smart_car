/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file debug_display.c
 *
 * @par dependencies
 * - debug_display.h
 *
 * @author GS_Mark
 *
 * @brief Debug display service implementation.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include <stdint.h>
#include "debug_display.h"
#include "control.h"
#include "display.h"
#include "pal_display.h"
#include "sensor.h"
#include "vision.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define DEBUG_DISPLAY_IMAGE_X             0
#define DEBUG_DISPLAY_IMAGE_Y             0
#define DEBUG_DISPLAY_IMAGE_WIDTH         94
#define DEBUG_DISPLAY_IMAGE_HEIGHT        60
#define DEBUG_DISPLAY_ENCODER_LABEL_X     0
#define DEBUG_DISPLAY_ENCODER_VALUE_X     50
#define DEBUG_DISPLAY_LEFT_ENCODER_Y      80
#define DEBUG_DISPLAY_RIGHT_ENCODER_Y     60
#define DEBUG_DISPLAY_PID_LABEL_X         0
#define DEBUG_DISPLAY_PID_VALUE_X         50
#define DEBUG_DISPLAY_LEFT_PID_Y          100
#define DEBUG_DISPLAY_RIGHT_PID_Y         120
#define DEBUG_DISPLAY_ERROR_Y             140
#define DEBUG_DISPLAY_ENCODER_DIGITS      4
#define DEBUG_DISPLAY_PID_DIGITS          6
#define DEBUG_DISPLAY_ERROR_DIGITS        4
//******************************** Defines **********************************//

//******************************** Implement ********************************//
/**
 * @brief 绘制视觉边线与中线。
 *
 * 处理步骤：
 *  1. 读取视觉服务输出的左边线、右边线和中线数组。
 *  2. 委托 BSP 显示模块完成像素绘制。
 *
 * @return void : 无返回值。
 *
 * */
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
 * @brief 刷新 TFT 调试显示。
 *
 * 处理步骤：
 *  1. 显示压缩灰度图。
 *  2. 显示编码器实测速度、电机 PID 输出和转向误差。
 *
 * @param[in] events : 调度器事件掩码，当前未使用。
 *
 * @return void : 无返回值。
 *
 * */
void DebugDisplayService_Update(uint32_t events)
{
    vision_debug_snapshot_t vision_snapshot = {0};
    control_output_t control_output = {0.0f, 0.0f, 0.0f};

    (void)events;

    Vision_GetDebugSnapshot(&vision_snapshot);
    Control_GetOutputSnapshot(&control_output);

    pal_disp_gray(DEBUG_DISPLAY_IMAGE_X,
                  DEBUG_DISPLAY_IMAGE_Y,
                  vision_snapshot.p_binary_zip[0],
                  vision_snapshot.image_width,
                  vision_snapshot.image_height,
                  vision_snapshot.image_width,
                  vision_snapshot.image_height,
                  0);

    pal_disp_str(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_LEFT_ENCODER_Y, "left:");
    pal_disp_int(DEBUG_DISPLAY_ENCODER_VALUE_X,
                 DEBUG_DISPLAY_LEFT_ENCODER_Y,
                 SensorService_GetLeftEncoderSpeed(),
                 DEBUG_DISPLAY_ENCODER_DIGITS);

    pal_disp_str(DEBUG_DISPLAY_ENCODER_LABEL_X, DEBUG_DISPLAY_RIGHT_ENCODER_Y, "right:");
    pal_disp_int(DEBUG_DISPLAY_ENCODER_VALUE_X,
                 DEBUG_DISPLAY_RIGHT_ENCODER_Y,
                 SensorService_GetRightEncoderSpeed(),
                 DEBUG_DISPLAY_ENCODER_DIGITS);

    pal_disp_str(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_LEFT_PID_Y, "l_spd:");
    pal_disp_str(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_RIGHT_PID_Y, "r_spd:");
    pal_disp_int(DEBUG_DISPLAY_PID_VALUE_X,
                 DEBUG_DISPLAY_LEFT_PID_Y,
                 (int32_t)control_output.left_motor,
                 DEBUG_DISPLAY_PID_DIGITS);
    pal_disp_int(DEBUG_DISPLAY_PID_VALUE_X,
                 DEBUG_DISPLAY_RIGHT_PID_Y,
                 (int32_t)control_output.right_motor,
                 DEBUG_DISPLAY_PID_DIGITS);

    pal_disp_str(DEBUG_DISPLAY_PID_LABEL_X, DEBUG_DISPLAY_ERROR_Y, "err:");
    pal_disp_int(DEBUG_DISPLAY_PID_VALUE_X,
                 DEBUG_DISPLAY_ERROR_Y,
                 vision_snapshot.calculate_error,
                 DEBUG_DISPLAY_ERROR_DIGITS);
}
