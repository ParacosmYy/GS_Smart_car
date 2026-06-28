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

/* ======================== Camera ======================== */
typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} camera_frame_desc_t;

typedef struct {
    void     (*init)(void);
    bool     (*ready)(void);
    void     (*clear)(void);
    uint8_t *(*data)(void);
    void     (*get_frame_desc)(camera_frame_desc_t *p_desc);
} camera_ops_t;

void     Device_CameraRegister(const camera_ops_t *p_ops);
void     Device_CameraInit(void);
bool     Device_CameraReady(void);
void     Device_CameraClear(void);
uint8_t *Device_CameraData(void);
void     Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc);

/* ======================== Display ======================== */
typedef struct {
    void     (*init)(void);
    void     (*point)(int16_t x, int16_t y, uint16_t color);
    uint16_t (*width)(void);
    uint16_t (*height)(void);
    void     (*gray)(int16_t x, int16_t y, const uint8_t *p_img,
                     uint16_t w, uint16_t h,
                     uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
    void     (*str)(int16_t x, int16_t y, const char *p_str);
    void     (*int_val)(int16_t x, int16_t y, int32_t value, uint8_t digits);
} display_ops_t;

void     Device_DisplayRegister(const display_ops_t *p_ops);
void     Device_DisplayInit(void);
void     Device_DisplayPoint(int16_t x, int16_t y, uint16_t color);
uint16_t Device_DisplayWidth(void);
uint16_t Device_DisplayHeight(void);
void     Device_DisplayGray(int16_t x, int16_t y, const uint8_t *p_img,
                            uint16_t w, uint16_t h,
                            uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void     Device_DisplayStr(int16_t x, int16_t y, const char *p_str);
void     Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits);

/* ======================== IMU (Gyro) ======================== */
typedef struct {
    void  (*init)(void);
    void  (*read)(void);
    float (*z)(void);
} imu_ops_t;

void  Device_ImuRegister(const imu_ops_t *p_ops);
void  Device_ImuInit(void);
void  Device_ImuRead(void);
float Device_ImuZ(void);

/* ======================== Wireless ======================== */
typedef struct {
    void (*init)(void);
    void (*rx_handler)(void);
} wireless_ops_t;

void Device_WirelessRegister(const wireless_ops_t *p_ops);
void Device_WirelessInit(void);
void Device_WirelessRxHandler(void);

/* ======================== GNSS ======================== */
typedef void (*gnss_rx_callback_t)(void);
void Device_GnssSetRxCallback(gnss_rx_callback_t cb);
void Device_GnssRxCallback(void);

/* ======================== Key ======================== */
typedef struct {
    void (*init)(uint32_t period_ms);
} key_ops_t;

void Device_KeyRegister(const key_ops_t *p_ops);
void Device_KeyInit(uint32_t period_ms);

/* ======================== 诊断 ======================== */
/** @brief Device 全组件已注册返回 1，否则 0。*/
uint8_t Device_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_INTERFACE_DEVICE_IF_H_ */
