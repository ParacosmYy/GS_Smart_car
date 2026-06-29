#ifndef CODE_VISION_H_
#define CODE_VISION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VISION_RAW_W  188
#define VISION_RAW_H  120
#define VISION_ZIP_IMAGE_H 60
#define VISION_ZIP_IMAGE_W 94

typedef struct
{
    int16_t calculate_error;
    uint8_t lost_count;
    uint8_t mid_line;
    uint8_t image_mid;
    uint8_t image_threshold;
} vision_control_snapshot_t;

typedef struct
{
    const uint8_t (*p_binary_zip)[VISION_ZIP_IMAGE_W];
    const uint8_t *p_left_line;
    const uint8_t *p_right_line;
    const uint8_t *p_mid_line;
    uint16_t image_width;
    uint16_t image_height;
    uint8_t line_count;
    int16_t calculate_error;
} vision_debug_snapshot_t;

/* Public API */
void Vision_Process(void);
uint8_t Vision_IsFrameReady(void);
void Vision_MarkFrameConsumed(void);
void Vision_GetControlSnapshot(vision_control_snapshot_t *p_snapshot);
void Vision_GetDebugSnapshot(vision_debug_snapshot_t *p_snapshot);
uint8_t Vision_DetectElement(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_VISION_H_ */
