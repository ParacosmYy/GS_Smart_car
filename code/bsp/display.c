/**
 * @file display.c
 * @brief 调试屏赛道线绘制 BSP。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本模块只把视觉层给出的左右边线和中线绘制到屏幕，不持有视觉状态。
 */

#include "display.h"
#include "platform/port_if.h"

#define DISPLAY_TRACK_BLIND_ROWS        10U
#define DISPLAY_BOLD_POINT_RADIUS       1

/**
 * @brief 绘制 3x3 加粗点。
 *
 * Steps:
 *   1. 以输入坐标为中心遍历 3x3 邻域。
 *   2. 跳过下界外的邻域点，避免边缘坐标下溢。
 *   3. 调用显示端口逐点绘制。
 *
 * @param[in] x 中心点 X 坐标。
 * @param[in] y 中心点 Y 坐标。
 * @param[in] color RGB565 颜色。
 * @return void。
 */
static void Display_DrawPointBold(uint8_t x, uint8_t y, uint16_t color)
{
    int8_t dx;
    int8_t dy;
    int16_t point_x;
    int16_t point_y;

    for (dy = -DISPLAY_BOLD_POINT_RADIUS; dy <= DISPLAY_BOLD_POINT_RADIUS; dy++)
    {
        for (dx = -DISPLAY_BOLD_POINT_RADIUS; dx <= DISPLAY_BOLD_POINT_RADIUS; dx++)
        {
            point_x = (int16_t)x + dx;
            point_y = (int16_t)y + dy;

            if ((point_x >= 0) && (point_y >= 0))
            {
                Device_DisplayPoint(point_x, point_y, color);
            }
        }
    }
}

/**
 * @brief 绘制压缩图像中的左右边线和中线。
 *
 * Steps:
 *   1. 校验三条线指针和可绘制行数。
 *   2. 跳过底部盲区，从近端向远端绘制。
 *   3. 左线黄色、右线蓝色、中线红色加粗。
 *
 * @param[in] p_left_line 左边线数组。
 * @param[in] p_right_line 右边线数组。
 * @param[in] p_mid_line 中线数组。
 * @param[in] line_count 线数组有效行数。
 * @return void。
 */
void Display_DrawTrackLines(const uint8_t *p_left_line,
                            const uint8_t *p_right_line,
                            const uint8_t *p_mid_line,
                            uint8_t line_count)
{
    uint8_t i;

    if ((p_left_line == 0) || (p_right_line == 0) ||
        (p_mid_line == 0) || (line_count <= DISPLAY_TRACK_BLIND_ROWS))
    {
        return;
    }

    for (i = (uint8_t)(line_count - 1U); i > DISPLAY_TRACK_BLIND_ROWS; i--)
    {
        Device_DisplayPoint(p_left_line[i], i, RGB565_YELLOW);
        Device_DisplayPoint(p_right_line[i], i, RGB565_BLUE);
        Display_DrawPointBold(p_mid_line[i], i, RGB565_RED);
    }
}
