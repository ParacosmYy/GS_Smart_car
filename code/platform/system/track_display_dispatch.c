/**
 * @file track_display_dispatch.c
 * @brief 赛道显示 ops dispatch — TrackDisplay_* API。
 */
#include "platform/interface/track_display_if.h"
static const track_display_ops_t *s_ops = 0;
void TrackDisplay_Register(const track_display_ops_t *p) { s_ops = p; }
void TrackDisplay_DrawLines(const uint8_t *l, const uint8_t *r, const uint8_t *m, uint8_t c)
{ if (s_ops && s_ops->draw_lines) s_ops->draw_lines(l, r, m, c); }
