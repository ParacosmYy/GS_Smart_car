#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * @file platform.h
 * @brief PAL 兼容聚合头。
 *
 * 新代码应优先包含具体 PAL 能力头，例如 pal_gpio.h、pal_pwm.h、
 * pal_camera.h。保留本文件仅用于兼容历史 include。
 */

#include "pal_camera.h"
#include "pal_comm.h"
#include "pal_display.h"
#include "pal_encoder.h"
#include "pal_gpio.h"
#include "pal_imu.h"
#include "pal_key.h"
#include "pal_pit.h"
#include "pal_pwm.h"
#include "pal_system.h"
#include "pal_uart.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* PLATFORM_H_ */
