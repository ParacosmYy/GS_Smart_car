#ifndef SMARTCAR_SYSTEM_H_
#define SMARTCAR_SYSTEM_H_

/**
 * @file smartcar_system.h
 * @brief Smart car system runtime entry.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 执行系统启动编排 */
void SmartcarSystem_Boot(void);

/** @brief 执行一次系统调度 */
void SmartcarSystem_RunOnce(void);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_SYSTEM_H_ */
