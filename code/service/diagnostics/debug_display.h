/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file debug_display.h
 *
 * @par dependencies
 * - event.h
 *
 * @author GS_Mark
 *
 * @brief Debug display service interface.
 *
 * @version V1.0 2026-06-29
 *
 *****************************************************************************/
#ifndef DEBUG_DISPLAY_H_
#define DEBUG_DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Includes **********************************//
#include "event.h"
//******************************* Includes **********************************//

//******************************* Declaring *********************************//
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
