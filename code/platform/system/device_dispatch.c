/**
 * @file device_dispatch.c
 * @brief Device dispatch + 就绪诊断。
 */

#include "platform/interface/device_if.h"

static const camera_ops_t   *s_camera   = 0;
static const display_ops_t  *s_display  = 0;
static const imu_ops_t      *s_imu      = 0;
static const wireless_ops_t *s_wireless = 0;
static const key_ops_t      *s_key      = 0;
static gnss_rx_callback_t    s_gnss_cb  = 0;

void Device_CameraRegister(const camera_ops_t *p)   { s_camera = p; }
void Device_DisplayRegister(const display_ops_t *p) { s_display = p; }
void Device_ImuRegister(const imu_ops_t *p)         { s_imu = p; }
void Device_WirelessRegister(const wireless_ops_t *p) { s_wireless = p; }
void Device_KeyRegister(const key_ops_t *p)         { s_key = p; }

void Device_CameraInit(void)                 { if (s_camera && s_camera->init) s_camera->init(); }
bool Device_CameraReady(void)                { return (s_camera && s_camera->ready) ? s_camera->ready() : false; }
void Device_CameraClear(void)                { if (s_camera && s_camera->clear) s_camera->clear(); }
uint8_t *Device_CameraData(void)             { return (s_camera && s_camera->data) ? s_camera->data() : 0; }
void Device_CameraGetFrameDesc(camera_frame_desc_t *d) { if (s_camera && s_camera->get_frame_desc) s_camera->get_frame_desc(d); }
void Device_DisplayInit(void)                { if (s_display && s_display->init) s_display->init(); }
void Device_DisplayPoint(int16_t x, int16_t y, uint16_t c) { if (s_display && s_display->point) s_display->point(x, y, c); }
uint16_t Device_DisplayWidth(void)           { return (s_display && s_display->width) ? s_display->width() : 0; }
uint16_t Device_DisplayHeight(void)          { return (s_display && s_display->height) ? s_display->height() : 0; }
void Device_DisplayGray(int16_t x, int16_t y, const uint8_t *p, uint16_t w, uint16_t h, uint16_t dw, uint16_t dh, uint8_t t)
{ if (s_display && s_display->gray) s_display->gray(x, y, p, w, h, dw, dh, t); }
void Device_DisplayStr(int16_t x, int16_t y, const char *s) { if (s_display && s_display->str) s_display->str(x, y, s); }
void Device_DisplayInt(int16_t x, int16_t y, int32_t v, uint8_t d) { if (s_display && s_display->int_val) s_display->int_val(x, y, v, d); }
void Device_ImuInit(void)                    { if (s_imu && s_imu->init) s_imu->init(); }
void Device_ImuRead(void)                    { if (s_imu && s_imu->read) s_imu->read(); }
float Device_ImuZ(void)                      { return (s_imu && s_imu->z) ? s_imu->z() : 0.0f; }
void Device_WirelessInit(void)               { if (s_wireless && s_wireless->init) s_wireless->init(); }
void Device_WirelessRxHandler(void)          { if (s_wireless && s_wireless->rx_handler) s_wireless->rx_handler(); }
void Device_GnssSetRxCallback(gnss_rx_callback_t cb) { s_gnss_cb = cb; }
void Device_GnssRxCallback(void)             { if (s_gnss_cb) s_gnss_cb(); }
void Device_KeyInit(uint32_t ms)             { if (s_key && s_key->init) s_key->init(ms); }

/* ── 诊断 ────────────────────────────────────── */
uint8_t Device_IsReady(void)
{
    return (s_camera != 0) && (s_display != 0) && (s_imu != 0)
        && (s_wireless != 0) && (s_key != 0) ? 1U : 0U;
}
