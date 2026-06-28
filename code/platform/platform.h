#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * @file platform.h
 * @brief PAL 兼容聚合头。
 *
 * 新代码应优先包含具体 interface 头文件。
 */

#include "platform/interface/mcu_io_if.h"
#include "platform/interface/device_if.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* PLATFORM_H_ */
