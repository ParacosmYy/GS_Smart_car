/*
 * draw.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */
#include "zf_common_headfile.h"


#ifndef CODE_DRAW_H_
#define CODE_DRAW_H_

/**
 * @brief 八邻域画点（3x3区域），带越界保护
 */
void draw_piont(uint8_t x , uint8_t y ,const uint16_t color);

/**
 * @brief 在TFT上绘制赛道左/右/中线（调试显示）
 */
void draw_all_lines_test(void);
#endif /* CODE_DRAW_H_ */
