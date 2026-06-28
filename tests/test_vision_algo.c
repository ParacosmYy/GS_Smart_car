/**
 * @file test_vision_algo.c
 * @brief Host-side tests for vision algorithms.
 *
 * Compile:
 *   gcc -Itests/stubs -Icode -Icode/config \
 *       -Icode/service/vision -Icode/common -DVISION_ENABLE_TEST_ACCESS \
 *       tests/test_vision_algo.c code/service/vision/vision.c \
 *       code/common/utils.c -o test_vision_algo.exe
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef VISION_ENABLE_TEST_ACCESS
#define VISION_ENABLE_TEST_ACCESS
#endif
#include "platform/interface/device_if.h"
#include "vision.h"

/* Stub Device_Camera* */
uint8_t g_stub_cam_data[VISION_RAW_H][VISION_RAW_W] = {{0}};
bool Device_CameraReady(void) { return true; }
void Device_CameraClear(void) {}
uint8_t *Device_CameraData(void) { return (uint8_t *)g_stub_cam_data; }
void Device_CameraGetFrameDesc(camera_frame_desc_t *p) {
    if (p) { p->width = VISION_RAW_W; p->height = VISION_RAW_H; p->stride = VISION_RAW_W; }
}

static void fill_stripe(void) {
    for (int r = 0; r < VISION_RAW_H; r++)
        for (int c = 0; c < VISION_RAW_W; c++)
            g_stub_cam_data[r][c] = (c < 80) ? 40 : 200;
}
static void fill_straight_track(void) {
    uint8_t (*z)[VISION_ZIP_IMAGE_W] = Vision_GetZipBuffer();
    for (int r = 0; r < VISION_ZIP_IMAGE_H; r++)
        for (int c = 0; c < VISION_ZIP_IMAGE_W; c++)
            z[r][c] = (c >= 30 && c <= 60) ? 0 : 255;
}

static void test_otsu(void) {
    fill_stripe();
    uint8_t t = vision_otsu(Device_CameraData(), VISION_RAW_W, VISION_RAW_H);
    assert(t > 80 && t < 160);
    printf("  PASS otsu (threshold=%u)\n", t);
}
static void test_otsu_black(void) {
    memset(g_stub_cam_data, 0, sizeof(g_stub_cam_data));
    (void)vision_otsu(Device_CameraData(), VISION_RAW_W, VISION_RAW_H);
    printf("  PASS otsu_all_black\n");
}
static void test_binary_zip(void) {
    fill_stripe();
    uint8_t t = vision_otsu(Device_CameraData(), VISION_RAW_W, VISION_RAW_H);
    vision_set_image_grayscale_to_binary(t);
    assert(Vision_GetBinaryBuffer()[0][0] == 0);
    assert(Vision_GetBinaryBuffer()[0][VISION_RAW_W - 1] == 255);
    vision_zip_image();
    assert(Vision_GetZipBuffer()[0][0] == 0);
    assert(Vision_GetZipBuffer()[0][VISION_ZIP_IMAGE_W - 1] == 255);
    printf("  PASS binary_and_zip\n");
}
static void test_filter(void) {
    uint8_t (*z)[VISION_ZIP_IMAGE_W] = Vision_GetZipBuffer();
    memset(z, 255, (size_t)(VISION_ZIP_IMAGE_H * VISION_ZIP_IMAGE_W));
    z[5][5] = 0;
    vision_bin_image_filter();
    assert(Vision_GetZipBuffer()[5][5] == 255);
    printf("  PASS filter_noise\n");
}
static void test_find_mid(void) {
    fill_straight_track();
    vision_find_mid_line();
    for (int r = 0; r < VISION_ZIP_IMAGE_H; r++) {
        assert(Vision_GetLeftLine()[r] >= 28 && Vision_GetLeftLine()[r] <= 32);
        assert(Vision_GetRightLine()[r] >= 58 && Vision_GetRightLine()[r] <= 62);
        assert(Vision_GetMidLine()[r] >= 43 && Vision_GetMidLine()[r] <= 47);
    }
    printf("  PASS find_mid_line\n");
}
static void test_mid_weight(void) {
    fill_straight_track();
    vision_find_mid_line();
    vision_find_mid_line_weight();
    int16_t e = Vision_GetCalculateError();
    assert(e >= -5 && e <= 0);
    assert(Vision_GetLostCount() == 0);
    printf("  PASS mid_line_weight (error=%d)\n", e);
}
static void test_element_none(void) {
    fill_straight_track();
    vision_find_mid_line();
    assert(Vision_DetectElement() == 0);
    printf("  PASS detect_element_none\n");
}

int main(void) {
    printf("=== Vision Tests ===\n");
    test_otsu(); test_otsu_black(); test_binary_zip(); test_filter();
    test_find_mid(); test_mid_weight(); test_element_none();
    printf("=== ALL PASS ===\n");
    return 0;
}
