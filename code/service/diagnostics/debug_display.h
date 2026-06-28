/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file debug_display.h
 *
 * @par 依赖
 * - event.h
 *
 * @author GS_Mark
 *
 * @brief 调试显示服务接口
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef DEBUG_DISPLAY_H_
#define DEBUG_DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* 包含文件 **********************************//
#include "event.h"
//******************************* 包含文件 **********************************//

//******************************* 接口声明 **********************************//
/**
 * @brief 绘制视觉边线与中线
 */
void DebugDisplay_DrawVisionLines(void);

/**
 * @brief 刷新调试显示
 */
void DebugDisplay_Update(event_mask_t events);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_DISPLAY_H_ */
