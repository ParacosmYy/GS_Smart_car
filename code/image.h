/*
 * image.h
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */
#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_

#include "zf_common_headfile.h"
#include "draw.h"

#define zip_MT9V03X_H 60
#define zip_MT9V03X_W 94

extern uint8_t mt9v03x_image_bandw[MT9V03X_H][MT9V03X_W];
extern uint8_t mt9v03x_image_bandw_zip[60][94];
extern uint8_t image_threshold ;

extern uint8_t image_mid ;//图像中点
extern uint8_t mid_line ; //最后的加权平均中值
extern int16_t calculate_error; //加权平均中线 与 图像中线差值
extern int8_t left_line_lost_flag;
extern int8_t right_line_lost_flag;
extern uint8_t lost_count; // 丢线计数

extern uint8_t left_line_list[zip_MT9V03X_H]  ;//记录左三个边界点
extern uint8_t right_line_list[zip_MT9V03X_H] ;//记录右三个边界点
extern uint8_t mid_line_list[zip_MT9V03X_H] ; //记录中点

void set_image_grayscale_to_binary(uint8_t value); //灰度转二值化
void Bin_Image_Filter (void) ;//过滤噪点 （黑四周有2白 -》 白 ）
uint8_t otsu(uint8_t *image, uint16_t col, uint16_t row); //大津法动态获取图像黑白分割阈值
void find_mid_line(void);//寻找左右边界点
void find_mid_line_weight(void);//图像中值 与 补线中值差 然后 计算平均偏差数
void task_calculte(void);//找边界 计算权重中值 画线 显示 集合体

#endif /* CODE_IMAGE_H_ */
