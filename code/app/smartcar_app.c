/*
 * smartcar_app.c
 *  应用层主循环实现（事件驱动 + 协作式调度器）
 *
 *  ISR 只做最少工作（采样 + 置事件），处理逻辑由调度器分发到各任务：
 *    task_gyro      陀螺仪采样 + 零漂补偿 + 角度积分（事件触发）
 *    task_encoder   编码器测速平均（事件触发）
 *    task_vision    视觉处理 + 元素检测 + 蜂鸣器（事件触发）
 *    task_control   控制 PID + 执行器下发（10ms 周期）
 *    task_display   TFT 调试显示（100ms 周期）
 */
#include "smartcar_app.h"
#include "platform.h"
#include "control.h"
#include "buzzer.h"
#include "scheduler.h"
#include "event.h"
#include "data.h"       /* z_angle, dt, GYRO_OFFSET_BUF_SIZE */

/* ---- 编码器速度（task_encoder 写入，control 模块读取）----
   原定义在 isr.c，现随 Encoder_CalculateSpeed 逻辑迁入此处。*/
int left_encoder_speed  = 0;
int right_encoder_speed = 0;

/* ---- 编码器测速累加器（定义在 isr.c，由 ISR 累加、task_encoder 清零）---- */
extern int       left_speed_sum;
extern int       right_speed_sum;
extern int       sample_count;
extern const int MAX_SAMPLES;

/* ---- 陀螺仪零漂补偿变量（原 isr.c 中 Gyro_CompensateDrift 的状态）---- */
#define GYRO_IDLE_THRESHOLD  1.0f   /* 静止判定阈值（°/s），可按实际调 0.5~2 */

static float   gyro_z_offset_buf[GYRO_OFFSET_BUF_SIZE] = {0.0f};
static uint8_t gyro_z_offset_idx = 0;
static float   gyro_z_offset_sum = 0.0f;
static float   gyro_z_offset     = 0.0f;
static float   gyro_raw_z        = 0.0f;

/**
 * @brief 陀螺仪处理任务（原 ISR 中的 Gyro_CompensateDrift + Gyro_Integrate 逻辑）
 *        EVT_GYRO_10MS 触发，10ms 周期。
 */
static void task_gyro(event_mask_t events)
{
    if (!(events & EVT_GYRO_10MS)) return;

    /* 采样 + 零漂补偿 */
    pal_gyro_read();
    gyro_raw_z = pal_gyro_z();

    if (fabsf(gyro_raw_z) < GYRO_IDLE_THRESHOLD)
    {
        gyro_z_offset_sum -= gyro_z_offset_buf[gyro_z_offset_idx];
        gyro_z_offset_buf[gyro_z_offset_idx] = gyro_raw_z;
        gyro_z_offset_sum += gyro_raw_z;

        gyro_z_offset_idx++;
        if (gyro_z_offset_idx >= GYRO_OFFSET_BUF_SIZE) gyro_z_offset_idx = 0;

        gyro_z_offset = gyro_z_offset_sum / GYRO_OFFSET_BUF_SIZE;
    }

    /* 角度积分：去零漂后的角速度乘以步长 dt 累加进航向角 */
    float z_angle_speed = gyro_raw_z - gyro_z_offset;
    z_angle += z_angle_speed * dt;
}

/**
 * @brief 编码器测速任务（原 ISR 中的 Encoder_CalculateSpeed 逻辑）
 *        EVT_ENCODER_50MS 触发，50ms 周期。
 */
static void task_encoder(event_mask_t events)
{
    if (!(events & EVT_ENCODER_50MS)) return;

    left_encoder_speed  = left_speed_sum  / MAX_SAMPLES;
    right_encoder_speed = right_speed_sum / MAX_SAMPLES;

    /* Reset accumulators for the next averaging window. */
    left_speed_sum  = 0;
    right_speed_sum = 0;
    sample_count    = 0;

    pal_encoder_clear(PAL_CH_ENCODER_L);
    pal_encoder_clear(PAL_CH_ENCODER_R);
}

/**
 * @brief 视觉处理任务
 *        EVT_CAM_FRAME 触发。处理一帧图像、检测特殊元素、蜂鸣器提示。
 */
static void task_vision(event_mask_t events)
{
    if (!(events & EVT_CAM_FRAME)) return;

    Vision_Process();

    /* 特殊元素检测 + 蜂鸣器提示 */
    uint8_t element = Vision_DetectElement();
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

    pal_cam_clear();
}

/**
 * @brief 控制任务（周期 10ms）
 *        基于视觉偏差与编码器速度计算 PID，并下发到执行器。
 */
static void task_control(event_mask_t events)
{
    (void)events;
    Control_Update();
    Actuator_Apply();
}

/**
 * @brief 显示任务（周期 100ms）
 *        刷新 TFT180 灰度图与关键调试数据。
 */
static void task_display(event_mask_t events)
{
    (void)events;

    /* 显示半分辨率灰度图（94×60 缩放窗口）*/
    pal_disp_gray(0, 0, mt9v03x_image_bandw_zip[0], 94, 60, PAL_CAM_W / 2, PAL_CAM_H / 2, 0);

    /* 左右编码器实测速度 */
    pal_disp_str(0, 80, "left:");
    pal_disp_int(50, 80, left_encoder_speed, 4);

    pal_disp_str(0, 60, "right:");
    pal_disp_int(50, 60, right_encoder_speed, 4);

    /* 左右电机 PID 输出 */
    pal_disp_str(0, 100, "l_spd:");
    pal_disp_str(0, 120, "r_spd:");
    pal_disp_int(50, 100, (int32_t)left_motor_pid_output, 6);
    pal_disp_int(50, 120, (int32_t)right_motor_pid_output, 6);

    /* 当前转向误差 */
    pal_disp_str(0, 140, "err:");
    pal_disp_int(50, 140, calculate_error, 4);
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
    scheduler_add(task_gyro,     0,   EVT_GYRO_10MS);    /* 陀螺仪：事件触发 */
    scheduler_add(task_encoder,  0,   EVT_ENCODER_50MS); /* 编码器：事件触发 */
    scheduler_add(task_vision,   0,   EVT_CAM_FRAME);    /* 视觉：事件触发   */
    scheduler_add(task_control,  10,  EVT_NONE);         /* 控制：10ms 周期  */
    scheduler_add(task_display,  100, EVT_NONE);         /* 显示：100ms 周期 */
}

/**
 * @brief 应用层单次循环
 *        检查摄像头帧就绪 → 置事件 → 调度器分发 → 蜂鸣器时序。
 */
void SmartcarApp_RunOnce(void)
{
    /* 摄像头帧就绪检查（逐飞库 DMA 回调设置的标志）*/
    if (pal_cam_ready())
    {
        event_set_isr(EVT_CAM_FRAME);
    }

    /* 调度器驱动全部任务 */
    scheduler_run();

    /* 蜂鸣器时序驱动（非阻塞）*/
    Buzzer_Tick();
}
