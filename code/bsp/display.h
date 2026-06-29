#ifndef CODE_BSP_DISPLAY_H_
#define CODE_BSP_DISPLAY_H_

#include <stdint.h>

#define RGB565_RED     0xF800
#define RGB565_GREEN   0x07E0
#define RGB565_BLUE    0x001F
#define RGB565_YELLOW  0xFFE0
#define RGB565_WHITE   0xFFFF
#define RGB565_BLACK   0x0000

void Display_DrawTrackLines(const uint8_t *p_left_line, const uint8_t *p_right_line,
                            const uint8_t *p_mid_line, uint8_t line_count);

#endif /* CODE_BSP_DISPLAY_H_ */
