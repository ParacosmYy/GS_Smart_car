/**
 * @file sensor_hal.h
 * @brief Stable platform sensor sampling contract.
 * @author GS_Mark
 */
#ifndef CODE_PLATFORM_SENSOR_HAL_H_
#define CODE_PLATFORM_SENSOR_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 读取并清零编码器采样窗口快照 */
void SensorHal_EncoderTakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#ifdef __cplusplus
}
#endif

#endif /* CODE_PLATFORM_SENSOR_HAL_H_ */
