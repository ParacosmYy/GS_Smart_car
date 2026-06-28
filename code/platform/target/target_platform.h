#ifndef PLATFORM_TARGET_PLATFORM_H_
#define PLATFORM_TARGET_PLATFORM_H_

/**
 * @file target_platform.h
 * @brief 目标平台注册入口（中性头文件）。
 *
 * System runtime 只调用 TargetPlatform_RegisterAll()，不 care 具体芯片型号。
 * Impl 层（如 impl/tc264/tc264_board_bind.c）实现此函数。
 *
 * 换 MCU：新建 impl/<target>/<target>_board_bind.c 实现此函数即可，
 * System/App/Service 零改动。
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 注册目标平台全部 ops 到 Platform dispatch 层。*/
void TargetPlatform_RegisterAll(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_TARGET_PLATFORM_H_ */
