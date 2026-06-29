/**
 * @file tc264_device_ops.c
 * @brief TC264 设备链接期端口实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件直接实现 Device_* 符号，把中性设备契约绑定到 SEEKFREE 设备库。
 */

#include "platform/interface/device_if.h"
#include "zf_common_headfile.h"

/**
 * @brief 初始化 MT9V03X 摄像头。
 *
 * Steps:
 *   1. 调用 SEEKFREE 摄像头初始化函数。
 *
 * @return void。
 */
void Device_CameraInit(void)
{
    mt9v03x_init();
}

/**
 * @brief 查询摄像头帧是否采集完成。
 *
 * @return true 表示有新帧；false 表示暂无新帧。
 */
bool Device_CameraReady(void)
{
    return (mt9v03x_finish_flag == 1);
}

/**
 * @brief 清除摄像头帧完成标志。
 *
 * Steps:
 *   1. 将 Vendor 帧完成标志复位为 0。
 *
 * @return void。
 */
void Device_CameraClear(void)
{
    mt9v03x_finish_flag = 0;
}

/**
 * @brief 获取摄像头灰度图像缓冲区。
 *
 * @return Vendor 摄像头图像缓冲区首地址。
 */
uint8_t *Device_CameraData(void)
{
    return (uint8_t *)mt9v03x_image;
}

/**
 * @brief 获取摄像头帧几何信息。
 *
 * Steps:
 *   1. 校验输出指针。
 *   2. 写入 MT9V03X 当前固定宽高和 stride。
 *
 * @param[out] p_desc 帧描述输出指针。
 * @return void。
 */
void Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc)
{
    if (p_desc != 0)
    {
        p_desc->width = 188U;
        p_desc->height = 120U;
        p_desc->stride = 188U;
    }
}

/**
 * @brief 初始化 TFT180 调试显示屏。
 *
 * @return void。
 */
void Device_DisplayInit(void)
{
    tft180_init();
}

/**
 * @brief 绘制单个屏幕像素。
 *
 * @param[in] x X 坐标。
 * @param[in] y Y 坐标。
 * @param[in] color RGB565 颜色。
 * @return void。
 */
void Device_DisplayPoint(int16_t x, int16_t y, uint16_t color)
{
    tft180_draw_point(x, y, color);
}

/**
 * @brief 显示灰度图像。
 *
 * Steps:
 *   1. 透传显示位置、图像尺寸和阈值参数到 Vendor 显示函数。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] p_img 灰度图像缓冲区。
 * @param[in] w 原图宽度。
 * @param[in] h 原图高度。
 * @param[in] dis_w 显示宽度。
 * @param[in] dis_h 显示高度。
 * @param[in] threshold 显示阈值。
 * @return void。
 */
void Device_DisplayGray(int16_t x,
                        int16_t y,
                        const uint8_t *p_img,
                        uint16_t w,
                        uint16_t h,
                        uint16_t dis_w,
                        uint16_t dis_h,
                        uint8_t threshold)
{
    tft180_show_gray_image(x, y, p_img, w, h, dis_w, dis_h, threshold);
}

/**
 * @brief 显示字符串。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] p_str 字符串指针。
 * @return void。
 */
void Device_DisplayStr(int16_t x, int16_t y, const char *p_str)
{
    tft180_show_string(x, y, p_str);
}

/**
 * @brief 显示整数。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] value 待显示整数。
 * @param[in] digits 显示位数。
 * @return void。
 */
void Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits)
{
    tft180_show_int(x, y, value, digits);
}

/**
 * @brief 初始化 ICM20602 IMU。
 *
 * @return void。
 */
void Device_ImuInit(void)
{
    icm20602_init();
}

/**
 * @brief 读取 IMU 陀螺仪原始数据。
 *
 * @return void。
 */
void Device_ImuRead(void)
{
    icm20602_get_gyro();
}

/**
 * @brief 获取 Z 轴角速度工程值。
 *
 * @return Z 轴角速度。
 */
float Device_ImuZ(void)
{
    return icm20602_gyro_transition(icm20602_gyro_z);
}

/**
 * @brief 初始化无线串口模块。
 *
 * @return void。
 */
void Device_WirelessInit(void)
{
    wireless_uart_init();
}
