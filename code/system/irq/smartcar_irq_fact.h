#ifndef SMARTCAR_IRQ_FACT_H_
#define SMARTCAR_IRQ_FACT_H_

/**
 * @file smartcar_irq_fact.h
 * @brief Smart car interrupt fact definitions.
 */

//******************************* Includes **********************************//
#include <stdint.h>
//******************************* Includes **********************************//

//******************************* Defines ***********************************//
typedef uint32_t smartcar_irq_fact_t;

#define SMARTCAR_IRQ_FACT_NONE            (0U)
#define SMARTCAR_IRQ_FACT_ENCODER_WINDOW  (1U << 0)
#define SMARTCAR_IRQ_FACT_GYRO_TICK       (1U << 1)
#define SMARTCAR_IRQ_FACT_CAMERA_FRAME    (1U << 2)
//******************************* Defines ***********************************//

#endif /* SMARTCAR_IRQ_FACT_H_ */
