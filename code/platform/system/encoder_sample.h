#ifndef ENCODER_SAMPLE_H_
#define ENCODER_SAMPLE_H_

/**
 * @file encoder_sample.h
 * @brief 编码器采样窗口快照接口。
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 获取并清零编码器累加快照 */
void EncoderSample_TakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_SAMPLE_H_ */
