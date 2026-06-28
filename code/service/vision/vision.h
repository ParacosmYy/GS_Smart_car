/*
 * vision.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * @brief 视觉处理模块接口
 *        流程：OTSU 二值化 → 压缩 → 滤波 → 边线检测 → 加权中线
 *        输出：calculate_error（PID 输入）、lost_count（停止判断）
 */
#ifndef CODE_VISION_H_
#define CODE_VISION_H_

#include "platform.h"

#define VISION_ZIP_IMAGE_H 60 // 压缩后图像高度
#define VISION_ZIP_IMAGE_W 94 // 压缩后图像宽度

typedef struct
{
    int16_t calculate_error;   // 中线偏差，供控制器作为 PID 输入
    uint8_t lost_count;        // 连续丢线计数，超阈值则停止控制
    uint8_t mid_line;          // 加权平均后的最终中线位置
    uint8_t image_mid;         // 图像参考中点列
    uint8_t image_threshold;   // 当前帧二值化阈值
} vision_control_snapshot_t;

typedef struct
{
    const uint8_t (*p_binary_zip)[VISION_ZIP_IMAGE_W]; // 压缩后二值图
    const uint8_t *p_left_line;                   // 每行左边界列坐标
    const uint8_t *p_right_line;                  // 每行右边界列坐标
    const uint8_t *p_mid_line;                    // 每行中线列坐标
    uint16_t image_width;                         // 压缩图像宽度
    uint16_t image_height;                        // 压缩图像高度
    uint8_t line_count;                           // 边线数组长度
    int16_t calculate_error;                      // 调试显示用偏差
} vision_debug_snapshot_t;

/** @brief 灰度图按阈值转二值图（黑=0 赛道，白=255 背景） */
void set_image_grayscale_to_binary(uint8_t value); //灰度转二值化
/** @brief 4 邻域多数表决滤波，去除孤立噪点 */
void Bin_Image_Filter (void) ;//过滤噪点 （黑四周有2白 -》 白 ）
/** @brief OTSU 大津法：计算使类间方差最大的二值化阈值 */
uint8_t otsu(uint8_t *image, uint16_t col, uint16_t row); //大津法动态获取图像黑白分割阈值
/** @brief 从底向顶逐行检测左右边线，计算每行中线（含时间平滑） */
void find_mid_line(void);//寻找左右边界点
/** @brief 行加权平均得到最终中线 mid_line，并输出偏差 calculate_error 与丢线计数 lost_count */
void find_mid_line_weight(void);//图像中值 与 补线中值差 然后 计算平均偏差数
/** @brief 视觉处理总入口：OTSU → 二值化 → 压缩 → 滤波 → 边线 → 加权中线 → 画线 */
void Vision_Process(void);//找边界 计算权重中值 画线 显示 集合体
/** @brief 摄像头是否已有新帧 */
uint8_t Vision_IsFrameReady(void);
/** @brief 清除摄像头帧就绪标志 */
void Vision_ClearFrameReady(void);
/** @brief 读取控制层所需的完整帧快照 */
void Vision_GetControlSnapshot(vision_control_snapshot_t *p_snapshot);
/** @brief 读取调试显示所需的视觉快照 */
void Vision_GetDebugSnapshot(vision_debug_snapshot_t *p_snapshot);
/** @brief 获取中线偏差 */
int16_t Vision_GetCalculateError(void);
/** @brief 获取连续丢线计数 */
uint8_t Vision_GetLostCount(void);

/**
 * @brief 检测特殊赛道元素（圆环 / 十字路口）
 * @return 0=无特殊元素, 1=圆环, 2=十字路口
 * @note 基于 left_line_list / right_line_list 的启发式判断，带冷却防抖
 */
uint8_t Vision_DetectElement(void);

#endif /* CODE_VISION_H_ */
