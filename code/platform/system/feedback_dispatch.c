/**
 * @file feedback_dispatch.c
 * @brief 反馈 ops dispatch — Feedback_* API。
 */
#include "platform/interface/feedback_if.h"
static const feedback_ops_t *s_ops = 0;
void Feedback_Register(const feedback_ops_t *p) { s_ops = p; }
void Feedback_NotifyElement(uint8_t e) { if (s_ops && s_ops->notify_element) s_ops->notify_element(e); }
uint8_t Feedback_IsBusy(void)          { return (s_ops && s_ops->is_busy) ? s_ops->is_busy() : 0U; }
void Feedback_Tick(void)               { if (s_ops && s_ops->tick) s_ops->tick(); }
