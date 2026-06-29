/**
 * @file smartcar_irq_router.c
 * @brief System IRQ fact 到事件映射实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * TC264 ISR adapter 只返回 IRQ_FACT_*；本文件负责把事实转换为系统事件和调度 tick。
 */

#include "smartcar_irq_router.h"

#include "config.h"
#include "event.h"
#include "scheduler.h"

/**
 * @brief 将 ISR adapter 返回的事实投递到系统调度出口。
 *
 * Steps:
 *   1. 编码器窗口就绪时投递 EVT_ENCODER_50MS。
 *   2. 陀螺 tick 时增加调度器时间并投递 EVT_GYRO_10MS。
 *   3. 摄像头帧完成时投递 EVT_CAM_FRAME。
 *
 * @param[in] facts ISR adapter 返回的事实掩码。
 * @return void。
 */
void SmartcarIrq_PostFacts(irq_fact_t facts)
{
    if ((facts & IRQ_FACT_ENCODER_WINDOW) != 0U)
    {
        event_post_from_isr(EVT_ENCODER_50MS);
    }

    if ((facts & IRQ_FACT_GYRO_TICK) != 0U)
    {
        Scheduler_AddTickFromIsr(PIT_PERIOD_MS);
        event_post_from_isr(EVT_GYRO_10MS);
    }

    if ((facts & IRQ_FACT_CAMERA_FRAME) != 0U)
    {
        event_post_from_isr(EVT_CAM_FRAME);
    }
}
