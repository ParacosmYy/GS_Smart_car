/*
 * smartcar_app.c
 *  应用层主循环实现
 *
 *  实现三步流水线：
 *    Vision_Process()  → 处理摄像头一帧图像，提取中线与误差
 *    Control_Update()  → 基于误差更新舵机/电机 PID 输出
 *    Actuator_Apply()  → 把 PID 输出下发给舵机与电机硬件
 *
 *  此外在每轮循环中刷新 TFT180 屏幕，用于在线调试。
 */
#include "smartcar_app.h"
#include "zf_common_headfile.h"
#include "control.h"

/**
 * @brief 应用层初始化入口
 * 顺序：先初始化全部外设，再启动周期中断。
 */
void SmartcarApp_Init(void)
{
    init_all();
    pit_init_all();
}

/**
 * @brief 应用层单次循环
 *
 * 由主循环周期性调用。摄像头一帧采集完成时触发完整流水线，
 * 否则跳过控制环节，仅刷新调试显示，避免空跑。
 */
void SmartcarApp_RunOnce(void)
{
    // ===== Vision → Control → Actuator 三步流水线 =====
    // 仅在新一帧图像就绪时执行，保证控制周期与帧率同步
    if(mt9v03x_finish_flag == 1)
    {
        Vision_Process();    // 步骤 1：视觉处理，从图像提取赛道中线与转向误差
        Control_Update();    // 步骤 2：控制更新，PID 计算舵机和电机目标值
        Actuator_Apply();    // 步骤 3：执行下发，将目标值写入 PWM 寄存器
        mt9v03x_finish_flag = 0;   // 清除帧完成标志，等待下一帧
    }

    // ===== TFT180 调试显示 =====
    // 实时显示图像与关键状态，便于现场调参与排错

    // 显示半分辨率灰度图（94×60 缩放窗口）
    tft180_show_gray_image(0, 0, mt9v03x_image_bandw_zip[0], 94, 60, MT9V03X_W / 2, MT9V03X_H / 2, 0);

    // 左右编码器实测速度（来自控制模块）
    tft180_show_string(0, 80, "left:");
    tft180_show_int(50, 80, left_encoder_speed, 4);

    tft180_show_string(0, 60, "right:");
    tft180_show_int(50, 60, right_encoder_speed, 4);

    // 左右电机 PID 输出（下发给驱动器的目标值）
    tft180_show_string(0, 100, "l_spd:");
    tft180_show_string(0, 120, "r_spd:");

    tft180_show_int(50, 100, (int32)left_motor_pid_output, 6);
    tft180_show_int(50, 120, (int32)right_motor_pid_output, 6);

    // 当前转向误差（视觉模块计算结果）
    tft180_show_string(0, 140, "err:");
    tft180_show_int(50, 140, calculate_error, 4);
}
