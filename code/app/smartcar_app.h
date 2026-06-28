/*
 * smartcar_app.h
 * @brief Smart car application lifecycle interface.
 *
 *  对外暴露两个入口：
 *    SmartcarApp_Init()    系统启动时调用一次
 *    SmartcarApp_RunOnce() 主循环中反复调用
 */
#ifndef CODE_APP_SMARTCAR_APP_H_
#define CODE_APP_SMARTCAR_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 应用层初始化
 *
 * 串联 init_all() 与 pit_init_all()，完成全部外设和周期中断配置。
 * 应在主任务进入循环前调用一次。
 */
void SmartcarApp_Init(void);

/**
 * @brief 应用层单次循环
 *
 * The app scheduler drives SensorService / Vision / Control /
 * DebugDisplayService, then applies actuator output through BSP modules.
 * 由主循环周期性调用。
 */
void SmartcarApp_RunOnce(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_APP_SMARTCAR_APP_H_ */
