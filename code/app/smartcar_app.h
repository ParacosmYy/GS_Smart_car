/*
 * smartcar_app.h
 * @brief Smart car application task interface.
 *
 *  对外暴露两个入口：
 *    SmartcarApp_Init()    注册应用任务
 *    SmartcarApp_RunOnce() 主循环中反复调用
 */
#ifndef CODE_APP_SMARTCAR_APP_H_
#define CODE_APP_SMARTCAR_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 应用层初始化，只注册业务任务 */
void SmartcarApp_Init(void);

/** @brief 应用层单次循环 */
void SmartcarApp_RunOnce(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_APP_SMARTCAR_APP_H_ */
