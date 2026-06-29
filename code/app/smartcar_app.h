/**
 * @file smartcar_app.h
 * @brief Smart car application task registration interface.
 * @author GS_Mark
 */
#ifndef CODE_APP_SMARTCAR_APP_H_
#define CODE_APP_SMARTCAR_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 注册应用层业务任务，仅在 scheduler_init() 后启动期调用一次 */
void SmartcarApp_RegisterTasks(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_APP_SMARTCAR_APP_H_ */
