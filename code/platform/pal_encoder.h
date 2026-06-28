#ifndef PAL_ENCODER_H_
#define PAL_ENCODER_H_

/**
 * @file pal_encoder.h
 * @brief PAL 编码器接口。
 */

#include <stdint.h>
#include "pal_resources.h"

void    pal_encoder_init(pal_encoder_id_t ch);
int32_t pal_encoder_get(pal_encoder_id_t ch);
void    pal_encoder_clear(pal_encoder_id_t ch);
void    pal_encoder_take_snapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#endif /* PAL_ENCODER_H_ */
