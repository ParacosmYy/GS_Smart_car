/*
 * data.h
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 *
 * @brief Legacy global runtime data declarations.
 *
 * 新代码应优先通过 service/context API 访问运行态数据。
 */

#ifndef CODE_DATA_H_
#define CODE_DATA_H_

#include <stdint.h>

extern volatile int pit_ch0_count  ;       // CCU60 通道 0 中断计数（兼容保留）


//extern uint16_t count  ;



#endif /* CODE_DATA_H_ */
