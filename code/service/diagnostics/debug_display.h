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
#ifndef CODE_SERVICE_DIAGNOSTICS_DEBUG_DISPLAY_H_
#define CODE_SERVICE_DIAGNOSTICS_DEBUG_DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

//******************************* Includes **********************************//
#include <stdint.h>
//******************************* Includes **********************************//

/**
 * @brief 刷新调试显示
 */
void DebugDisplayService_Update(uint32_t events);

#ifdef __cplusplus
}
#endif

#endif /* CODE_SERVICE_DIAGNOSTICS_DEBUG_DISPLAY_H_ */
