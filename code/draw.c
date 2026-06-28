/*
 * draw.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */
#include "draw.h"

#define zip_MT9V03X_H 60
#define zip_MT9V03X_W 94


void draw_piont(uint8_t x, uint8_t y, uint16_t color) //八邻域画点(防止超出屏幕)
{
    int dx[] = {0, 1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 0, 1, 1, 1, 0, -1, -1, -1};
    for (int i = 0; i < 9; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < tft180_width_max && ny >= 0 && ny < tft180_height_max)
        {
            tft180_draw_point(nx, ny, color);
        }
    }
}



void draw_all_lines_test(void)
{
    for(uint8_t i = zip_MT9V03X_H - 1; i > 10 ; i--)
        {
            // 左边界线：红色
        tft180_draw_point(left_line_list[i], i, RGB565_YELLOW);
            // 右边界线：蓝色
        tft180_draw_point(right_line_list[i], i, RGB565_BLUE);
            // 中线：黄色
        draw_piont(mid_line_list[i], i, RGB565_RED);
        //tft180_draw_point(mid_line_list[i], i, RGB565_YELLOW);
        }
}
