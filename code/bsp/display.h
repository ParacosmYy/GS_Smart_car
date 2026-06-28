/*
 * display.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */
#ifndef CODE_DRAW_H_
#define CODE_DRAW_H_

#include <stdint.h>
#include "platform.h"

/* RGB565 颜色常量（显示驱动专有参数，原逐飞库提供，此处本地定义） */
#define RGB565_RED     0xF800
#define RGB565_GREEN   0x07E0
#define RGB565_BLUE    0x001F
#define RGB565_YELLOW  0xFFE0
#define RGB565_WHITE   0xFFFF
#define RGB565_BLACK   0x0000

/**
 * @brief 在TFT上绘制赛道左/右/中线（调试显示）
 * @param p_left_line  左边线数组
 * @param p_right_line 右边线数组
 * @param p_mid_line   中线数组
 * @param line_count   数组长度
 */
void Display_DrawTrackLines(const uint8_t *p_left_line,
                            const uint8_t *p_right_line,
                            const uint8_t *p_mid_line,
                            uint8_t line_count);
#endif /* CODE_DRAW_H_ */
