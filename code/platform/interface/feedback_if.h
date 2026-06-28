#ifndef FEEDBACK_IF_H_
#define FEEDBACK_IF_H_

/* 蜂鸣器反馈领域契约 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*notify_element)(uint8_t element);
    uint8_t (*is_busy)(void);
    void (*tick)(void);
} feedback_ops_t;

void Feedback_Register(const feedback_ops_t *p_ops);
void Feedback_NotifyElement(uint8_t element);
uint8_t Feedback_IsBusy(void);
void Feedback_Tick(void);

#ifdef __cplusplus
}
#endif
#endif
