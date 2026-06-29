#ifndef SCHEDULER_ENCODER_SAMPLE_H_
#define SCHEDULER_ENCODER_SAMPLE_H_

/**
 * @file encoder_sample.h
 * @brief 编码器采样窗口快照协同接口。
 * @author GS_Mark
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 读取并清零编码器采样窗口快照 */
void EncoderSample_TakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_ENCODER_SAMPLE_H_ */
