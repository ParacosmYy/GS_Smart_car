/**
 * @file tc264_device_ops.c
 * @brief TC264 设备 ops 聚合 —— Camera / Display / IMU / Wireless / Key。
 */

#include "platform/interface/device_if.h"
#include "zf_common_headfile.h"

/* ======================== Camera ======================== */
static void tc264_camera_init(void) { mt9v03x_init(); }
static bool tc264_camera_ready(void) { return (mt9v03x_finish_flag == 1); }
static void tc264_camera_clear(void) { mt9v03x_finish_flag = 0; }
static uint8_t *tc264_camera_data(void) { return (uint8_t *)mt9v03x_image; }
static void tc264_camera_get_frame_desc(camera_frame_desc_t *d)
{ if (d) { d->width = 188; d->height = 120; d->stride = 188; } }
const camera_ops_t g_tc264_camera_ops = { .init = tc264_camera_init, .ready = tc264_camera_ready, .clear = tc264_camera_clear, .data = tc264_camera_data, .get_frame_desc = tc264_camera_get_frame_desc };

/* ======================== Display ======================== */
static void tc264_display_init(void) { tft180_init(); }
static void tc264_display_point(int16_t x, int16_t y, uint16_t c) { tft180_draw_point(x, y, c); }
static uint16_t tc264_display_width(void) { return (uint16_t)tft180_width_max; }
static uint16_t tc264_display_height(void) { return (uint16_t)tft180_height_max; }
static void tc264_display_gray(int16_t x, int16_t y, const uint8_t *p, uint16_t w, uint16_t h, uint16_t dw, uint16_t dh, uint8_t t)
{ tft180_show_gray_image(x, y, p, w, h, dw, dh, t); }
static void tc264_display_str(int16_t x, int16_t y, const char *s) { tft180_show_string(x, y, s); }
static void tc264_display_int(int16_t x, int16_t y, int32_t v, uint8_t d) { tft180_show_int(x, y, v, d); }
const display_ops_t g_tc264_display_ops = { .init = tc264_display_init, .point = tc264_display_point, .width = tc264_display_width, .height = tc264_display_height, .gray = tc264_display_gray, .str = tc264_display_str, .int_val = tc264_display_int };

/* ======================== IMU ======================== */
static void tc264_imu_init(void) { icm20602_init(); }
static void tc264_imu_read(void) { icm20602_get_gyro(); }
static float tc264_imu_z(void) { return icm20602_gyro_transition(icm20602_gyro_z); }
const imu_ops_t g_tc264_imu_ops = { .init = tc264_imu_init, .read = tc264_imu_read, .z = tc264_imu_z };

/* ======================== Wireless ======================== */
static void tc264_wireless_init(void) { wireless_uart_init(); }
static void tc264_wireless_rx_handler(void) { wireless_module_uart_handler(); }
const wireless_ops_t g_tc264_wireless_ops = { .init = tc264_wireless_init, .rx_handler = tc264_wireless_rx_handler };

/* ======================== Key ======================== */
static void tc264_key_init(uint32_t ms) { key_init(ms); }
const key_ops_t g_tc264_key_ops = { .init = tc264_key_init };
