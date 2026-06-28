#ifndef VISION_CONTEXT_H_
#define VISION_CONTEXT_H_

#include "vision.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t binary[VISION_RAW_H][VISION_RAW_W];
    uint8_t binary_zip[VISION_ZIP_IMAGE_H][VISION_ZIP_IMAGE_W];
    uint8_t left_line[VISION_ZIP_IMAGE_H];
    uint8_t right_line[VISION_ZIP_IMAGE_H];
    uint8_t mid_line_per_row[VISION_ZIP_IMAGE_H];
    uint8_t  mid_line;
    uint8_t  image_mid;
    int16_t  calculate_error;
    uint8_t  lost_count;
    uint8_t  image_threshold;
    uint8_t element_cooldown;
} vision_context_t;

void Vision_InitContext(vision_context_t *p_ctx);

#ifdef __cplusplus
}
#endif

#endif /* VISION_CONTEXT_H_ */
