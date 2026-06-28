/*
 * draw.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * 注：本文件例外保留逐飞头文件。PAL 未提供像素级绘制接口
 *     （tft180_draw_point）与屏幕尺寸常量（tft180_width_max /
 *     tft180_height_max），draw_piont / draw_all_lines_test 仍需调用
 *     逐飞 TFT 底层接口。颜色常量已移至 display.h，不再依赖逐飞头文件。
 */
#include "display.h"
#include "zf_common_headfile.h"   /* 仅用于 tft180_draw_point 等像素级接口 */

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
    for (int i = 0; i < 9; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
        // 越界检查，只绘制屏幕范围内的点
        if (nx >= 0 && nx < tft180_width_max && ny >= 0 && ny < tft180_height_max)
        {
            tft180_draw_point(nx, ny, color);
        }
    }
}



/**
 * @brief 在TFT屏幕上绘制赛道边线和中线（调试显示）
 *        黄色=左边界，蓝色=右边界，红色=中线（加粗）
 */
void draw_all_lines_test(void)
{
    // 从图像底部向上扫描，跳过最近10行（车头盲区）
    for(uint8_t i = zip_MT9V03X_H - 1; i > 10 ; i--)
        {
            // 左边界线：黄色
        tft180_draw_point(left_line_list[i], i, RGB565_YELLOW);
            // 右边界线：蓝色
        tft180_draw_point(right_line_list[i], i, RGB565_BLUE);
            // 中线：红色（八邻域加粗）
        draw_piont(mid_line_list[i], i, RGB565_RED);
        //tft180_draw_point(mid_line_list[i], i, RGB565_YELLOW);
        }
}
