#ifndef PAL_DISPLAY_H_
#define PAL_DISPLAY_H_

/**
 * @file pal_display.h
 * @brief PAL 显示接口。
 */

#include <stdint.h>

void     pal_disp_init(void);
void     pal_disp_point(int16_t x, int16_t y, uint16_t color);
uint16_t pal_disp_width(void);
uint16_t pal_disp_height(void);
void     pal_disp_gray(int16_t x, int16_t y, const uint8_t *p_img,
                       uint16_t w, uint16_t h,
                       uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void     pal_disp_str(int16_t x, int16_t y, const char *p_str);
void     pal_disp_int(int16_t x, int16_t y, int32_t value, uint8_t digits);

#endif /* PAL_DISPLAY_H_ */
