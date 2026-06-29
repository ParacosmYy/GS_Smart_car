#ifndef SYSTEM_PORT_H_
#define SYSTEM_PORT_H_

/**
 * @file system_port.h
 * @brief 系统时钟、调试和全局中断控制端口。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 初始化系统时钟 */
void SystemPort_ClockInit(void);

/** @brief 初始化调试通道 */
void SystemPort_DebugInit(void);

/** @brief 等待多核启动同步 */
void SystemPort_CoreSync(void);

/** @brief 关闭全局中断并返回恢复状态 */
uint32_t SystemPort_IrqGlobalDisable(void);

/** @brief 恢复全局中断状态 */
void SystemPort_IrqGlobalRestore(uint32_t state);

/** @brief 设置全局中断开关状态 */
void SystemPort_IrqGlobalCtrl(uint8_t state);

/** @brief 读取并清零编码器窗口采样 */
void EncoderSample_TakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_PORT_H_ */
