/*
 * smartcar_app.c
 * @brief Smart car application scheduler implementation.
 *
 * Processing flow:
 * ISR entries do minimum bounded work and task processing is dispatched by
 * the cooperative scheduler:
 *    SmartcarApp_TaskGyro      陀螺仪传感器服务处理（事件触发）
 *    SmartcarApp_TaskEncoder   编码器传感器服务处理（事件触发）
 *    SmartcarApp_TaskVision    视觉处理 + 元素检测 + 蜂鸣器（事件触发）
 *    SmartcarApp_TaskControl   控制 PID + 执行器下发（10ms 周期）
 *    DebugDisplayService_Update TFT 调试显示（100ms 周期）
 */
#include "smartcar_app.h"
#include "control.h"
#include "buzzer.h"
#include "debug_display.h"
#include "init.h"
#include "scheduler.h"
#include "event.h"
#include "sensor.h"
#include "vision.h"

/**
 * @brief 陀螺仪处理任务
 *        EVT_GYRO_10MS 触发，10ms 周期。
 */
static void SmartcarApp_TaskGyro(event_mask_t events)
{
    if ((events & EVT_GYRO_10MS) == 0U)
    {
        return;
    }

    SensorService_ProcessGyro10ms();
}

/**
 * @brief 编码器测速任务（原 ISR 中的 Encoder_CalculateSpeed 逻辑）
 *        EVT_ENCODER_50MS 触发，50ms 周期。
 */
static void SmartcarApp_TaskEncoder(event_mask_t events)
{
    if ((events & EVT_ENCODER_50MS) == 0U)
    {
        return;
    }

    SensorService_ProcessEncoder50ms();
}

/**
 * @brief 视觉处理任务
 *        EVT_CAM_FRAME 触发。处理一帧图像、检测特殊元素、蜂鸣器提示。
 */
static void SmartcarApp_TaskVision(event_mask_t events)
{
    uint8_t element = 0;

    if ((events & EVT_CAM_FRAME) == 0U)
    {
        return;
    }

    Vision_Process();
    DebugDisplayService_DrawVisionLines();

    /* 特殊元素检测 + 蜂鸣器提示 */
    element = Vision_DetectElement();
    if (element != 0 && !Buzzer_IsBusy())
    {
        if (element == 1)
        {
            Buzzer_Trigger(BUZZER_EVENT_RING);       /* 圆环：3 声短促 */
        }
        else if (element == 2)
        {
            Buzzer_Trigger(BUZZER_EVENT_CROSSROAD);  /* 十字路口：1 声长鸣 */
        }
    }

    Vision_ClearFrameReady();
}

/**
 * @brief 控制任务（周期 10ms）
 *        基于视觉偏差与编码器速度计算 PID，并下发到执行器。
 */
static void SmartcarApp_TaskControl(event_mask_t events)
{
    (void)events;
    Control_Update();
    Actuator_Apply();
}

/**
 * @brief 应用层初始化入口
 *        初始化外设、启动周期中断、注册调度器任务。
 */
void SmartcarApp_Init(void)
{
    init_all();
    pit_init_all();
    scheduler_init();

    /* 注册任务（按优先级从高到低）*/
    scheduler_add(SmartcarApp_TaskGyro,      0,   EVT_GYRO_10MS);    /* 陀螺仪：事件触发 */
    scheduler_add(SmartcarApp_TaskEncoder,   0,   EVT_ENCODER_50MS); /* 编码器：事件触发 */
    scheduler_add(SmartcarApp_TaskVision,    0,   EVT_CAM_FRAME);    /* 视觉：事件触发   */
    scheduler_add(SmartcarApp_TaskControl,   10,  EVT_NONE);         /* 控制：10ms 周期  */
    scheduler_add(DebugDisplayService_Update, 100, EVT_NONE);        /* 显示：100ms 周期 */
}

/**
 * @brief 应用层单次循环
 *        检查摄像头帧就绪 → 置事件 → 调度器分发 → 蜂鸣器时序。
 */
void SmartcarApp_RunOnce(void)
{
    /* 摄像头帧就绪检查。 */
    if (Vision_IsFrameReady() != 0U)
    {
        event_set_isr(EVT_CAM_FRAME);
    }

    /* 调度器驱动全部任务 */
    scheduler_run();

    /* 蜂鸣器时序驱动（非阻塞）*/
    Buzzer_Tick();
}
