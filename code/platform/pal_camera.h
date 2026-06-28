#ifndef PAL_CAMERA_H_
#define PAL_CAMERA_H_

/**
 * @file pal_camera.h
 * @brief PAL 摄像头接口。
 */

#include <stdbool.h>
#include <stdint.h>

#define PAL_CAM_W  188
#define PAL_CAM_H  120

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} pal_cam_frame_desc_t;

void     pal_cam_init(void);
bool     pal_cam_ready(void);
void     pal_cam_clear(void);
uint8_t *pal_cam_data(void);
void     pal_cam_get_frame_desc(pal_cam_frame_desc_t *p_desc);

#endif /* PAL_CAMERA_H_ */
