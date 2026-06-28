#ifndef PAL_IRQ_FACT_H_
#define PAL_IRQ_FACT_H_

/**
 * @file pal_irq_fact.h
 * @brief LEGACY 中断事实兼容接口。
 */

#include "irq_fact.h"

typedef irq_fact_t pal_irq_fact_t;

#define PAL_IRQ_FACT_NONE           IRQ_FACT_NONE
#define PAL_IRQ_FACT_ENCODER_WINDOW IRQ_FACT_ENCODER_WINDOW
#define PAL_IRQ_FACT_GYRO_TICK      IRQ_FACT_GYRO_TICK
#define PAL_IRQ_FACT_CAMERA_FRAME   IRQ_FACT_CAMERA_FRAME

#endif /* PAL_IRQ_FACT_H_ */
