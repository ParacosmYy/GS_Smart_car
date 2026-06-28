#ifndef IRQ_FACT_H_
#define IRQ_FACT_H_

/**
 * @file irq_fact.h
 * @brief 中断事实接口（Platform contract）。
 */

#include <stdint.h>

/** @brief 中断事实位掩码类型 */
typedef uint32_t irq_fact_t;

/** @brief 无事实 */
#define IRQ_FACT_NONE           (0U)
/** @brief 编码器测速窗口完成 */
#define IRQ_FACT_ENCODER_WINDOW (1U << 0)
/** @brief 陀螺仪 10ms tick */
#define IRQ_FACT_GYRO_TICK      (1U << 1)
/** @brief 摄像头帧 DMA 完成 */
#define IRQ_FACT_CAMERA_FRAME   (1U << 2)

#endif /* IRQ_FACT_H_ */
