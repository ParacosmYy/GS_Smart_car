/**
 * @file event.h
 * @brief Lightweight event flag system for ISR-to-main-loop notification.
 *
 *        ISR 调用 event_post_from_isr() 设置事件位
 *        主循环调用 event_get() 获取并清除全部待处理事件
 *        EVT_GYRO_10MS 使用计数语义，避免主循环阻塞时丢 tick
 *
 *        单生产者（ISR）→ 单消费者（主循环），无需 RTOS
 */
#ifndef CODE_SCHEDULER_EVENT_H_
#define CODE_SCHEDULER_EVENT_H_

#include <stdint.h>

typedef uint32_t event_mask_t;

/* 事件标志位定义 */
#define EVT_NONE          (0U)
#define EVT_CAM_FRAME     (1U << 0)   /* 摄像头新一帧就绪           */
#define EVT_ENCODER_50MS  (1U << 1)   /* 编码器 50ms 采样窗口完成   */
#define EVT_GYRO_10MS     (1U << 2)   /* 陀螺仪 10ms 采样时刻       */

/**
 * @brief ISR 中调用：设置事件标志（原子位或操作）
 * @param events 一个或多个 EVT_* 标志的位或
 */
void event_post_from_isr(event_mask_t events);

/**
 * @brief 主循环调用：获取并清除全部待处理事件
 *        计数事件每次调用只消费一个待处理 tick
 * @return 当前待处理的事件掩码（调用后全部清零）
 */
event_mask_t event_get(void);

/**
 * @brief 读取陀螺 10ms 事件计数溢出诊断标志
 * @return 1 表示曾发生计数饱和；0 表示未发生。
 */
uint8_t event_is_gyro_10ms_overflow_latched(void);

#endif /* CODE_SCHEDULER_EVENT_H_ */
