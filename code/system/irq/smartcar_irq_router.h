#ifndef SMARTCAR_IRQ_ROUTER_H_
#define SMARTCAR_IRQ_ROUTER_H_

#include "platform/target/target_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

void SmartcarIrq_Dispatch(smartcar_irq_source_t source);

#ifdef __cplusplus
}
#endif

#endif /* SMARTCAR_IRQ_ROUTER_H_ */
