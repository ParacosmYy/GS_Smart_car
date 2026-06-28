/*
 * display.c
 */
#include "display.h"
#include "platform/interface/device_if.h"
#define DISPLAY_BOLD_POINT_COUNT  9
#define DISPLAY_TRACK_BLIND_ROWS  10

static void Display_DrawPointBold(uint8_t x, uint8_t y, uint16_t color) {
    int8_t dx, dy;
    for (dy = -1; dy <= 1; dy++)
        for (dx = -1; dx <= 1; dx++)
            Device_DisplayPoint((int16_t)(uint16_t)x + dx, (int16_t)(uint16_t)y + dy, color);
}

void Display_DrawTrackLines(const uint8_t *p_left_line, const uint8_t *p_right_line,
                            const uint8_t *p_mid_line, uint8_t line_count) {
    if (!p_left_line || !p_right_line || !p_mid_line || line_count <= DISPLAY_TRACK_BLIND_ROWS) return;
    for (uint8_t i = (uint8_t)(line_count - 1); i > DISPLAY_TRACK_BLIND_ROWS; i--) {
        Device_DisplayPoint(p_left_line[i], i, RGB565_YELLOW);
        Device_DisplayPoint(p_right_line[i], i, RGB565_BLUE);
        Display_DrawPointBold(p_mid_line[i], i, RGB565_RED);
    }
}
