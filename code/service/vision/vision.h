/*
 * vision.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * @brief 视觉处理模块接口
 *        摄像头：MT9V03X（灰度，188×120）
 *        流程：OTSU 二值化 → 压缩(94×60) → 滤波 → 边线检测 → 加权中线
 *        输出：calculate_error（PID 输入）、lost_count（停止判断）
 */
#ifndef CODE_VISION_H_
#define CODE_VISION_H_

#include "platform.h"

#define zip_MT9V03X_H 60 // 压缩后图像高度
#define zip_MT9V03X_W 94 // 压缩后图像宽度

extern uint8_t mt9v03x_image_bandw[PAL_CAM_H][PAL_CAM_W]; // 原始尺寸二值图（188×120）
extern uint8_t mt9v03x_image_bandw_zip[60][94];           // 压缩后二值图（94×60）
extern uint8_t image_threshold ;                          // 当前帧二值化阈值（OTSU 输出）

extern uint8_t image_mid ;        // 图像参考中点列（47）
extern uint8_t mid_line ;         // 加权平均后的最终中线位置
extern int16_t calculate_error;   // 中线偏差（mid_line - image_mid），PID 输入
extern int8_t left_line_lost_flag;   // 左边线丢线标志（保留）
extern int8_t right_line_lost_flag;  // 右边线丢线标志（保留）
extern uint8_t lost_count;        // 连续丢线计数，超阈值则停止控制

// 按行存储的边线/中线数据（长度 60，索引 0=图像顶部，59=图像底部）
extern uint8_t left_line_list[zip_MT9V03X_H]  ; // 每行左边界列坐标
extern uint8_t right_line_list[zip_MT9V03X_H] ; // 每行右边界列坐标
extern uint8_t mid_line_list[zip_MT9V03X_H] ;   // 每行中线列坐标（左右均值）

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

/**
 * @brief 检测特殊赛道元素（圆环 / 十字路口）
 * @return 0=无特殊元素, 1=圆环, 2=十字路口
 * @note 基于 left_line_list / right_line_list 的启发式判断，带冷却防抖
 */
uint8_t Vision_DetectElement(void);

#endif /* CODE_VISION_H_ */
