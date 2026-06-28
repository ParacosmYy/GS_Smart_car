/*
 * draw.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * 注：显示层只依赖 platform PAL，不直接包含或调用逐飞 Vendor 接口。
 *     像素绘制与屏幕尺寸由 pal_disp_point / pal_disp_width /
 *     pal_disp_height 提供。
 */
#include "display.h"

// 压缩后图像尺寸（用于TFT显示坐标映射）
#define zip_MT9V03X_H 60   // 图像高度（行数）
#define zip_MT9V03X_W 94   // 图像宽度（列数）


/**
 * @brief 八邻域画点，在指定坐标周围3x3区域绘制同色像素
 *        通过边界检查避免越界写入 TFT 显存，同时让细线更醒目
 * @param x     列坐标
 * @param y     行坐标
 * @param color RGB565 颜色值
 */
void draw_piont(uint8_t x, uint8_t y, uint16_t color) //八邻域画点(防止超出屏幕)
{
    // 中心点 + 八个邻域方向偏移，共9个位置
    int dx[] = {0, 1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 0, 1, 1, 1, 0, -1, -1, -1};
    int width = (int)pal_disp_width();
    int height = (int)pal_disp_height();

    for (int i = 0; i < 9; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
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

    if ((p_left_line == 0) || (p_right_line == 0) || (p_mid_line == 0) || (line_count <= 10))
    {
        return;
    }

    /* 从图像底部向上扫描，跳过最近 10 行车头盲区。 */
    for (i = (uint8_t)(line_count - 1); i > 10 ; i--)
    {
        /* 左边界线：黄色 */
        pal_disp_point(p_left_line[i], i, RGB565_YELLOW);

        /* 右边界线：蓝色 */
        pal_disp_point(p_right_line[i], i, RGB565_BLUE);

        /* 中线：红色（八邻域加粗） */
        draw_piont(p_mid_line[i], i, RGB565_RED);
    }
}
