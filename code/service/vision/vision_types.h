#ifndef CODE_SERVICE_VISION_VISION_TYPES_H_
#define CODE_SERVICE_VISION_VISION_TYPES_H_

/**
 * @file vision_types.h
 * @brief 视觉服务共享轻量类型。
 * @author GS_Mark
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 赛道元素检测结果。 */
typedef enum
{
    VISION_TRACK_ELEMENT_NONE = 0,
    VISION_TRACK_ELEMENT_RING,
    VISION_TRACK_ELEMENT_CROSSROAD
} vision_track_element_t;

#ifdef __cplusplus
}
#endif

#endif /* CODE_SERVICE_VISION_VISION_TYPES_H_ */
