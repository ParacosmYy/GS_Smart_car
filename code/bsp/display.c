/*
 * display.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * @brief Display BSP implementation.
 *
 * The display layer depends only on the platform PAL. Pixel write and screen
 * geometry are provided by pal_disp_point / pal_disp_width / pal_disp_height,
 * so this module does not include SEEKFREE vendor headers directly.
 */
#include "display.h"

// 压缩后图像尺寸（用于TFT显示坐标映射）
#define zip_MT9V03X_H 60   // 图像高度（行数）
#define zip_MT9V03X_W 94   // 图像宽度（列数）
#define DISPLAY_BOLD_POINT_COUNT  9
#define DISPLAY_TRACK_BLIND_ROWS  10

static const int8_t s_point_dx[DISPLAY_BOLD_POINT_COUNT] = {0, 1, 1, 0, -1, -1, -1, 0, 1};
static const int8_t s_point_dy[DISPLAY_BOLD_POINT_COUNT] = {0, 0, 1, 1, 1, 0, -1, -1, -1};


/**
 * @brief 八邻域画点，在指定坐标周围3x3区域绘制同色像素
 *        通过边界检查避免越界写入 TFT 显存，同时让细线更醒目
 * @param x     列坐标
 * @param y     行坐标
 * @param color RGB565 颜色值
 */
static void Display_DrawPointBold(uint8_t x, uint8_t y, uint16_t color)
{
    uint8_t i = 0;
    int width = (int)pal_disp_width();
    int height = (int)pal_disp_height();
    int nx = 0;
    int ny = 0;

    // 中心点 + 八个邻域方向偏移，共 9 个位置。
    for (i = 0; i < DISPLAY_BOLD_POINT_COUNT; i++)
    {
        nx = (int)x + (int)s_point_dx[i];
        ny = (int)y + (int)s_point_dy[i];

        // 越界检查，只绘制屏幕范围内的点
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            pal_disp_point((int16_t)nx, (int16_t)ny, color);
        }
    }
}



/**
 * @brief 在TFT屏幕上绘制赛道边线和中线（调试显示）
 *        黄色=左边界，蓝色=右边界，红色=中线（加粗）
 */
void Display_DrawTrackLines(const uint8_t *p_left_line,
                            const uint8_t *p_right_line,
                            const uint8_t *p_mid_line,
                            uint8_t line_count)
{
    uint8_t i = 0;

    if ((p_left_line == 0) || (p_right_line == 0) || (p_mid_line == 0) || (line_count <= DISPLAY_TRACK_BLIND_ROWS))
    {
        return;
    }

    /* 从图像底部向上扫描，跳过最近 10 行车头盲区。 */
    for (i = (uint8_t)(line_count - 1); i > DISPLAY_TRACK_BLIND_ROWS ; i--)
    {
        /* 左边界线：黄色 */
        pal_disp_point(p_left_line[i], i, RGB565_YELLOW);

        /* 右边界线：蓝色 */
        pal_disp_point(p_right_line[i], i, RGB565_BLUE);

        /* 中线：红色（八邻域加粗） */
        Display_DrawPointBold(p_mid_line[i], i, RGB565_RED);
    }
}
