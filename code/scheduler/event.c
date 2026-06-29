/**
 * @file event.c
 * @brief Event flag system implementation.
 * @author GS_Mark
 *
 * @par 设计说明
 * 事件模块保存 ISR 投递的轻量事件标志。普通事件按位合并，陀螺 10ms 事件额外计数，
 * 避免主循环偶发延迟时丢失连续 tick。
 */
#include "event.h"
#include "platform/port_if.h"

static volatile event_mask_t s_events = 0;
static volatile uint32_t s_gyro_10ms_pending = 0U;
static volatile uint8_t s_gyro_10ms_overflow_latched = 0U;

/**
 * @brief 从 ISR 上下文投递事件。
 *
 * Steps:
 *   1. 进入全局临界区。
 *   2. 普通事件按位合并到 s_events。
 *   3. EVT_GYRO_10MS 使用独立计数器记录待处理次数。
 *   4. 若陀螺 tick 计数已饱和，则置位内部诊断标志。
 *   5. 恢复中断状态。
 *
 * @param[in] events 待投递事件掩码。
 * @return void。
 */
void event_post_from_isr(event_mask_t events)
{
    event_mask_t normal_events = EVT_NONE;
    uint32_t irq_state = 0;

    irq_state = SystemPort_IrqGlobalDisable();
    normal_events = events & (~EVT_GYRO_10MS);
    s_events |= normal_events;

    if ((events & EVT_GYRO_10MS) != 0U)
    {
        if (s_gyro_10ms_pending < UINT32_MAX)
        {
            s_gyro_10ms_pending++;
        }
        else
        {
            s_gyro_10ms_overflow_latched = 1U;
        }
    }
    SystemPort_IrqGlobalRestore(irq_state);
}

/**
 * @brief 获取并消费待处理事件。
 *
 * Steps:
 *   1. 进入全局临界区。
 *   2. 取出普通事件并清零。
 *   3. 若陀螺 tick 计数非零，则本次附带 EVT_GYRO_10MS 并递减计数。
 *   4. 恢复中断状态。
 *
 * @return 本轮调度要处理的事件掩码。
 */
event_mask_t event_get(void)
{
    event_mask_t pending = EVT_NONE;
    uint32_t irq_state = 0;

    irq_state = SystemPort_IrqGlobalDisable();
    pending = s_events;
    s_events = 0;

    if (s_gyro_10ms_pending > 0U)
    {
        pending |= EVT_GYRO_10MS;
        s_gyro_10ms_pending--;
    }
    SystemPort_IrqGlobalRestore(irq_state);

    return pending;
}

/**
 * @brief 读取陀螺 10ms 事件计数溢出诊断标志。
 *
 * @return 1 表示曾发生计数饱和；0 表示未发生。
 */
uint8_t event_is_gyro_10ms_overflow_latched(void)
{
    uint8_t overflow_latched = 0U;
    uint32_t irq_state = 0U;

    irq_state = SystemPort_IrqGlobalDisable();
    overflow_latched = s_gyro_10ms_overflow_latched;
    SystemPort_IrqGlobalRestore(irq_state);

    return overflow_latched;
}
