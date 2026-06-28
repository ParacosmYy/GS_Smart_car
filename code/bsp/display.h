/*
 * draw.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */
#ifndef CODE_DRAW_H_
#define CODE_DRAW_H_

#include "platform.h"

/* RGB565 颜色常量（显示驱动专有参数，原逐飞库提供，此处本地定义） */
#define RGB565_RED     0xF800
#define RGB565_GREEN   0x07E0
#define RGB565_BLUE    0x001F
#define RGB565_YELLOW  0xFFE0
#define RGB565_WHITE   0xFFFF
#define RGB565_BLACK   0x0000

/**
 * @brief 八邻域画点（3x3区域），带越界保护
 */
void draw_piont(uint8_t x , uint8_t y ,const uint16_t color);

/**
 * @brief 在TFT上绘制赛道左/右/中线（调试显示）
 */
void draw_all_lines_test(void);
#endif /* CODE_DRAW_H_ */
