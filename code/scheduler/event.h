/**
 * @file event.h
 * @brief 事件标志系统 —— ISR 通知主循环的轻量机制
 *
 *        ISR 调用 event_set_isr() 设置事件位
 *        主循环调用 event_get() 获取并清除全部待处理事件
 *
 *        单生产者（ISR）→ 单消费者（主循环），无需 RTOS
 */
#ifndef EVENT_H_
#define EVENT_H_

#include <stdint.h>

typedef uint32_t event_mask_t;

/* 事件标志位定义 */
#define EVT_NONE          (0U)
#define EVT_CAM_FRAME     (1U << 0)   /* 摄像头新一帧就绪           */
#define EVT_ENCODER_50MS  (1U << 1)   /* 编码器 50ms 采样窗口完成   */
#define EVT_GYRO_10MS     (1U << 2)   /* 陀螺仪 10ms 采样时刻       */
#define EVT_UART1_RX      (1U << 3)   /* UART1 收到数据             */
#define EVT_UART3_RX      (1U << 4)   /* UART3 收到数据             */

/**
 * @brief ISR 中调用：设置事件标志（原子位或操作）
 * @param events 一个或多个 EVT_* 标志的位或
 */
void event_set_isr(event_mask_t events);

/**
 * @brief 主循环调用：获取并清除全部待处理事件
 * @return 当前待处理的事件掩码（调用后全部清零）
 */
event_mask_t event_get(void);

#endif /* EVENT_H_ */
