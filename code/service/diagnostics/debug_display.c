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
#include "display.h"
#include "platform.h"
#include "pid.h"
#include "sensor.h"
#include "vision.h"
//******************************** Includes *********************************//

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
void DebugDisplay_DrawVisionLines(void)
{
    Display_DrawTrackLines(left_line_list, right_line_list, mid_line_list, zip_MT9V03X_H);
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
void DebugDisplay_Update(event_mask_t events)
{
    (void)events;

    pal_disp_gray(0, 0, mt9v03x_image_bandw_zip[0], 94, 60, PAL_CAM_W / 2, PAL_CAM_H / 2, 0);

    pal_disp_str(0, 80, "left:");
    pal_disp_int(50, 80, Sensor_GetLeftEncoderSpeed(), 4);

    pal_disp_str(0, 60, "right:");
    pal_disp_int(50, 60, Sensor_GetRightEncoderSpeed(), 4);

    pal_disp_str(0, 100, "l_spd:");
    pal_disp_str(0, 120, "r_spd:");
    pal_disp_int(50, 100, (int32_t)left_motor_pid_output, 6);
    pal_disp_int(50, 120, (int32_t)right_motor_pid_output, 6);

    pal_disp_str(0, 140, "err:");
    pal_disp_int(50, 140, calculate_error, 4);
}
