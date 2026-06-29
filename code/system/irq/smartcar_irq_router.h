#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

#include "platform/system/irq_fact.h"

#ifdef __cplusplus
extern "C" {
#endif

void SmartcarIrq_PostFacts(irq_fact_t facts);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
