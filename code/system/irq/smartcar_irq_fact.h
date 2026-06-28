#ifndef SMARTCAR_IRQ_FACT_H_
#define SMARTCAR_IRQ_FACT_H_

/**
 * @file smartcar_irq_fact.h
 * @brief LEGACY smart car interrupt fact aliases.
 *
 * 新代码应直接包含 platform/system/irq_fact.h。
 * 此文件仅用于兼容尚未迁移的代码路径。
 */

#include "platform/system/irq_fact.h"

#define SMARTCAR_IRQ_FACT_NONE           IRQ_FACT_NONE
#define SMARTCAR_IRQ_FACT_ENCODER_WINDOW IRQ_FACT_ENCODER_WINDOW
#define SMARTCAR_IRQ_FACT_GYRO_TICK      IRQ_FACT_GYRO_TICK
#define SMARTCAR_IRQ_FACT_CAMERA_FRAME   IRQ_FACT_CAMERA_FRAME

#endif /* SMARTCAR_IRQ_FACT_H_ */
