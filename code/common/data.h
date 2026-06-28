/*
 * data.h
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 *
 *  全局共享变量声明文件
 *  通过 extern 方式向外提供 data.c 中定义的全局变量，
 *  供 vision、control、actuator 等模块共享访问。
 */

#ifndef CODE_DATA_H_
#define CODE_DATA_H_

#include <stdint.h>

#define GYRO_OFFSET_BUF_SIZE 32   // 陀螺仪零漂校准缓冲区大小（采样点数）

extern volatile int pit_ch0_count  ;       // CCU60 通道 0 中断计数
extern volatile int pit_ch1_count   ;       // CCU60 通道 1 中断计数


//extern uint16_t count  ;



extern float z_angle ;      // 陀螺仪 Z 轴累计角度，单位：度（°）
extern float dt ;        // 控制周期（采样步长），单位：秒（s）

#endif /* CODE_DATA_H_ */
