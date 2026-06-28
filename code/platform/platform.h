#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * @file platform.h
 * @brief PAL 兼容聚合头。
 *
 * 新代码应优先包含具体 PAL 能力头，例如 pal_gpio.h、pal_pwm.h、
 * pal_camera.h。保留本文件仅用于兼容历史 include。
 */

#include "platform/device/pal_camera.h"
#include "platform/device/pal_comm.h"
#include "platform/device/pal_display.h"
#include "platform/device/pal_imu.h"
#include "platform/device/pal_key.h"
#include "platform/mcu/pal_encoder.h"
#include "platform/mcu/pal_gpio.h"
#include "platform/mcu/pal_pit.h"
#include "platform/mcu/pal_pwm.h"
#include "platform/mcu/pal_uart.h"
#include "platform/system/pal_system.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* PLATFORM_H_ */
