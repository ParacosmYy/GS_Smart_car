#include "isr_adapter.h"

#include "platform/interface/mcu_io_if.h"
#include "platform/system/encoder_sample.h"
#include "platform/system/system_port.h"
#include "system/board/smartcar_board_resources.h"
#include "zf_common_headfile.h"

#define ENCODER_WINDOW_SAMPLES 5

typedef struct
{
    volatile int left_sum;
    volatile int right_sum;
    volatile int samples;
    volatile uint8_t ready;
} encoder_window_t;

static encoder_window_t s_encoder_window = {0, 0, 0, 0U};

static void prepare_pit(uint16_t pit)
{
    SystemPort_IrqGlobalCtrl(0);
    McuIo_PitClearFlag(pit);
}

static void sample_encoder(void)
{
    s_encoder_window.left_sum += (int)McuIo_EncoderGet(SMARTCAR_ENCODER_LEFT);
    s_encoder_window.right_sum += (int)McuIo_EncoderGet(SMARTCAR_ENCODER_RIGHT);
    s_encoder_window.samples++;

    McuIo_EncoderClear(SMARTCAR_ENCODER_LEFT);
    McuIo_EncoderClear(SMARTCAR_ENCODER_RIGHT);
}

irq_fact_t IsrAdapter_Ccu60PitCh0(void)
{
    irq_fact_t fact = IRQ_FACT_NONE;

    prepare_pit(SMARTCAR_PIT_ENCODER_SAMPLE);
    sample_encoder();

    if ((s_encoder_window.samples >= ENCODER_WINDOW_SAMPLES) && (s_encoder_window.ready == 0U))
    {
        s_encoder_window.ready = 1U;
        fact = IRQ_FACT_ENCODER_WINDOW;
    }

    return fact;
}

irq_fact_t IsrAdapter_Ccu60PitCh1(void)
{
    prepare_pit(SMARTCAR_PIT_GYRO_TICK);
    return IRQ_FACT_GYRO_TICK;
}

irq_fact_t IsrAdapter_ExtiCh3Ch7(void)
{
    SystemPort_IrqGlobalCtrl(0);

    if (exti_flag_get(ERU_CH3_REQ6_P02_0))
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();
    }

    if (exti_flag_get(ERU_CH7_REQ16_P15_1))
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);
    }

    return IRQ_FACT_NONE;
}

irq_fact_t IsrAdapter_DmaCh5(void)
{
    SystemPort_IrqGlobalCtrl(0);
    camera_dma_handler();
    return IRQ_FACT_CAMERA_FRAME;
}

irq_fact_t IsrAdapter_Uart0Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);

#if DEBUG_UART_USE_INTERRUPT
    debug_interrupr_handler();
#endif

    return IRQ_FACT_NONE;
}

irq_fact_t IsrAdapter_Uart1Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);
    camera_uart_handler();
    return IRQ_FACT_NONE;
}

irq_fact_t IsrAdapter_Uart3Rx(void)
{
    SystemPort_IrqGlobalCtrl(0);
    wireless_module_uart_handler();
    return IRQ_FACT_NONE;
}

static irq_fact_t uart_error(IfxAsclin_Asc *p_handle)
{
    SystemPort_IrqGlobalCtrl(0);
    IfxAsclin_Asc_isrError(p_handle);
    return IRQ_FACT_NONE;
}

irq_fact_t IsrAdapter_Uart0Error(void)
{
    return uart_error(&uart0_handle);
}

irq_fact_t IsrAdapter_Uart1Error(void)
{
    return uart_error(&uart1_handle);
}

irq_fact_t IsrAdapter_Uart3Error(void)
{
    return uart_error(&uart3_handle);
}

void EncoderSample_TakeSnapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count)
{
    uint32_t irq_state;

    if ((p_left_sum == 0) || (p_right_sum == 0) || (p_sample_count == 0))
    {
        return;
    }

    irq_state = SystemPort_IrqGlobalDisable();

    *p_left_sum = s_encoder_window.left_sum;
    *p_right_sum = s_encoder_window.right_sum;
    if (s_encoder_window.samples > 0)
    {
        *p_sample_count = s_encoder_window.samples;
    }
    else
    {
        *p_sample_count = ENCODER_WINDOW_SAMPLES;
    }

    s_encoder_window.left_sum = 0;
    s_encoder_window.right_sum = 0;
    s_encoder_window.samples = 0;
    s_encoder_window.ready = 0U;

    SystemPort_IrqGlobalRestore(irq_state);
}
