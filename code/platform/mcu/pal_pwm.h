#ifndef PAL_PWM_H_
#define PAL_PWM_H_

/**
 * @file pal_pwm.h
 * @brief PAL PWM 接口。
 */

#include <stdint.h>
#include "platform/common/pal_resources.h"

void pal_pwm_init(pal_pwm_id_t ch, uint32_t freq_hz, uint32_t duty);
void pal_pwm_set_duty(pal_pwm_id_t ch, uint32_t duty);

#endif /* PAL_PWM_H_ */
