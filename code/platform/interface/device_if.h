#ifndef PLATFORM_INTERFACE_DEVICE_IF_H_
#define PLATFORM_INTERFACE_DEVICE_IF_H_

/**
 * @file device_if.h
 * @brief Platform 设备接口契约。
 *
 * 所有公开函数以 Device_ 前缀命名，避免与 Vendor SDK 同名函数冲突。
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} camera_frame_desc_t;

void     Device_CameraInit(void);
bool     Device_CameraReady(void);
void     Device_CameraClear(void);
uint8_t *Device_CameraData(void);
void     Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc);

void     Device_DisplayInit(void);
void     Device_DisplayPoint(int16_t x, int16_t y, uint16_t color);
void     Device_DisplayGray(int16_t x, int16_t y, const uint8_t *p_img,
                            uint16_t w, uint16_t h,
                            uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void     Device_DisplayStr(int16_t x, int16_t y, const char *p_str);
void     Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits);

void  Device_ImuInit(void);
void  Device_ImuRead(void);
float Device_ImuZ(void);

void Device_WirelessInit(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_INTERFACE_DEVICE_IF_H_ */
