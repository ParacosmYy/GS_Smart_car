#ifndef ISR_ADAPTER_H_
#define ISR_ADAPTER_H_

#include "platform/system/irq_fact.h"

irq_fact_t IsrAdapter_Ccu60PitCh0(void);
irq_fact_t IsrAdapter_Ccu60PitCh1(void);
irq_fact_t IsrAdapter_ExtiCh3Ch7(void);
irq_fact_t IsrAdapter_DmaCh5(void);
irq_fact_t IsrAdapter_Uart0Rx(void);
irq_fact_t IsrAdapter_Uart1Rx(void);
irq_fact_t IsrAdapter_Uart3Rx(void);
irq_fact_t IsrAdapter_Uart0Error(void);
irq_fact_t IsrAdapter_Uart1Error(void);
irq_fact_t IsrAdapter_Uart3Error(void);

#endif /* ISR_ADAPTER_H_ */
