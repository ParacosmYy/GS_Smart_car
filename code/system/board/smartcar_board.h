#ifndef SMARTCAR_BOARD_H_
#define SMARTCAR_BOARD_H_

/**
 * @file smartcar_board.h
 * @brief Smart car board startup interface.
 *
 * Board startup owns this car's device initialization sequence. App code only
 * registers business tasks and must not start hardware directly.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 初始化本车板级设备和控制 Handler */
void SmartcarBoard_InitDevices(void);

/** @brief 启动本车周期中断源 */
void SmartcarBoard_StartPeriodicIrq(void);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_BOARD_H_ */
