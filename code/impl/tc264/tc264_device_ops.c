/**
 * @file tc264_device_ops.c
 * @brief TC264 设备链接期端口实现。
 */

#include "platform/interface/device_if.h"
#include "zf_common_headfile.h"

void Device_CameraInit(void)
{
    mt9v03x_init();
}

bool Device_CameraReady(void)
{
    return (mt9v03x_finish_flag == 1);
}

void Device_CameraClear(void)
{
    mt9v03x_finish_flag = 0;
}

uint8_t *Device_CameraData(void)
{
    return (uint8_t *)mt9v03x_image;
}

void Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc)
{
    if (p_desc != 0)
    {
        p_desc->width = 188U;
        p_desc->height = 120U;
        p_desc->stride = 188U;
    }
}

void Device_DisplayInit(void)
{
    tft180_init();
}

void Device_DisplayPoint(int16_t x, int16_t y, uint16_t color)
{
    tft180_draw_point(x, y, color);
}

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

void Device_DisplayStr(int16_t x, int16_t y, const char *p_str)
{
    tft180_show_string(x, y, p_str);
}

void Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits)
{
    tft180_show_int(x, y, value, digits);
}

void Device_ImuInit(void)
{
    icm20602_init();
}

void Device_ImuRead(void)
{
    icm20602_get_gyro();
}

float Device_ImuZ(void)
{
    return icm20602_gyro_transition(icm20602_gyro_z);
}

void Device_WirelessInit(void)
{
    wireless_uart_init();
}
