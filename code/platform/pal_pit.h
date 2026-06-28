#ifndef PAL_PIT_H_
#define PAL_PIT_H_

/**
 * @file pal_pit.h
 * @brief PAL 周期中断接口。
 */

#include <stdint.h>
#include "pal_resources.h"

void pal_pit_init(pal_pit_id_t ch, uint32_t period_ms);
void pal_pit_clear_flag(pal_pit_id_t ch);

#endif /* PAL_PIT_H_ */
