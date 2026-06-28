#ifndef TRACK_DISPLAY_IF_H_
#define TRACK_DISPLAY_IF_H_

/* 赛道显示领域契约 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*draw_lines)(const uint8_t *p_left, const uint8_t *p_right, const uint8_t *p_mid, uint8_t count);
} track_display_ops_t;

void TrackDisplay_Register(const track_display_ops_t *p_ops);
void TrackDisplay_DrawLines(const uint8_t *p_left, const uint8_t *p_right, const uint8_t *p_mid, uint8_t count);

#ifdef __cplusplus
}
#endif
#endif
