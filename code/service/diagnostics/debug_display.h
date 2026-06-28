/******************************************************************************
 * Copyright (C) 2025 GS_Mark. All rights reserved.
 *
 * @file debug_display.h
 *
 * @par dependencies
 * - stdint.h
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
#include <stdint.h>
//******************************* Includes **********************************//

//******************************* Declaring *********************************//
/**
 * @brief 绘制视觉边线与中线
 */
void DebugDisplayService_DrawVisionLines(void);

/**
 * @brief 刷新调试显示
 */
void DebugDisplayService_Update(uint32_t events);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_DISPLAY_H_ */
