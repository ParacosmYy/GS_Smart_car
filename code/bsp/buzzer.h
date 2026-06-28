/*
 * buzzer.h
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 */

#ifndef CODE_BUZZER_H_
#define CODE_BUZZER_H_

#include "zf_common_headfile.h"

/**
 * @brief 打开蜂鸣器（P11_11 拉高）
 */
static inline void buzzer_on(void)  { gpio_high(P11_11); }

/**
 * @brief 关闭蜂鸣器（P11_11 拉低）
 */
static inline void buzzer_off(void) { gpio_low(P11_11);  }

/**
 * @brief 蜂鸣器初始化，配置 P11_11 为 GPO 推挽输出并拉低
 */
void buzzer_init(void);

#endif /* CODE_BUZZER_H_ */
