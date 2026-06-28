#ifndef ZF_COMMON_HEADFILE_H_
#define ZF_COMMON_HEADFILE_H_

/*
 * Host-side stubs for TC264 SEEKFREE vendor headers.
 * Used by tests/ to compile platform-independent code on the host.
 */

#include <stdint.h>
#include <stdbool.h>

typedef int16_t int16;

/* GPIO stubs */
#define GPO               0
#define GPI               1
#define GPIO_LOW          0
#define GPIO_HIGH         1
#define GPO_PUSH_PULL     0
#define GPI_PULL_UP       1

static inline void gpio_init(uint32_t pin, uint32_t mode, uint32_t level, uint32_t drive) { (void)pin; (void)mode; (void)level; (void)drive; }
static inline void gpio_high(uint32_t pin) { (void)pin; }
static inline void gpio_low(uint32_t pin)  { (void)pin; }
static inline uint32_t gpio_get_level(uint32_t pin) { (void)pin; return 0; }

/* PWM stubs */
static inline void pwm_init(uint32_t ch, uint32_t freq, uint32_t duty) { (void)ch; (void)freq; (void)duty; }
static inline void pwm_set_duty(uint32_t ch, uint32_t duty) { (void)ch; (void)duty; }

/* PIT stubs */
static inline void pit_ms_init(uint32_t ch, uint32_t ms) { (void)ch; (void)ms; }
static inline void pit_clear_flag(uint32_t ch) { (void)ch; }

/* Encoder stubs */
#define TIM2_ENCODER  0
#define TIM4_ENCODER  1
#define TIM2_ENCODER_CH1_P33_7  0
#define TIM2_ENCODER_CH2_P33_6  0
#define TIM4_ENCODER_CH1_P02_8  0
#define TIM4_ENCODER_CH2_P00_9  0

static inline void encoder_dir_init(uint32_t timer, uint32_t ch1, uint32_t ch2) { (void)timer; (void)ch1; (void)ch2; }
static inline int32_t encoder_get_count(uint32_t timer) { (void)timer; return 0; }
static inline void encoder_clear_count(uint32_t timer) { (void)timer; }

/* UART stubs */
#define UART_1  0
#define UART_3  1
#define UART1_TX_P02_2  0
#define UART1_RX_P02_3  0
#define UART3_TX_P15_7  0
#define UART3_RX_P20_3  0

static inline void uart_init(uint32_t ch, uint32_t baud, uint32_t tx, uint32_t rx) { (void)ch; (void)baud; (void)tx; (void)rx; }

/* PWM channel stubs */
#define ATOM0_CH1_P21_3  0
#define ATOM0_CH0_P21_2  0
#define ATOM0_CH3_P21_5  0
#define ATOM0_CH2_P21_4  0
#define ATOM1_CH1_P33_9  0

/* GPIO pin stubs */
#define P11_11   0
#define P20_6    0
#define P20_7    0
#define P20_8    0
#define P20_9    0
#define P15_5    0
#define P15_6    0
#define P15_8    0
#define P15_9    0

/* PIT channel stubs */
#define CCU60_CH0  0
#define CCU60_CH1  1
#define CCU61_CH0  2
#define CCU61_CH1  3

/* Camera stubs */
static uint8_t mt9v03x_finish_flag = 0;
static uint8_t mt9v03x_image[120 * 188] = {0};

static inline void mt9v03x_init(void) {}
static inline void camera_vsync_handler(void) {}
static inline void camera_dma_handler(void) {}
static inline void camera_uart_handler(void) {}

/* IMU stubs */
static float icm20602_gyro_z = 0.0f;
static inline void icm20602_init(void) {}
static inline void icm20602_get_gyro(void) {}
static inline float icm20602_gyro_transition(float z) { (void)z; return 0.0f; }

/* Display stubs */
static uint16_t tft180_width_max = 180;
static uint16_t tft180_height_max = 180;
static inline void tft180_init(void) {}
static inline void tft180_draw_point(int16_t x, int16_t y, uint16_t c) { (void)x; (void)y; (void)c; }
static inline void tft180_show_gray_image(int16_t x, int16_t y, const uint8_t *img, uint16_t w, uint16_t h, uint16_t dw, uint16_t dh, uint8_t t) { (void)x; (void)y; (void)img; (void)w; (void)h; (void)dw; (void)dh; (void)t; }
static inline void tft180_show_string(int16_t x, int16_t y, const char *s) { (void)x; (void)y; (void)s; }
static inline void tft180_show_int(int16_t x, int16_t y, int32_t v, uint8_t d) { (void)x; (void)y; (void)v; (void)d; }

/* Wireless stubs */
static inline void wireless_uart_init(void) {}
static inline void wireless_module_uart_handler(void) {}
static inline void gnss_uart_callback(void) {}

/* IRQ / system stubs */
static inline void clock_init(void) {}
static inline void debug_init(void) {}
static inline void cpu_wait_event_ready(void) {}
static inline uint32_t interrupt_global_disable(void) { return 0; }
static inline void interrupt_global_enable(uint32_t s) { (void)s; }

/* Key stubs */
static inline void key_init(uint32_t period_ms) { (void)period_ms; }

/* EXTI stubs */
#define ERU_CH0_REQ0_P15_4   0
#define ERU_CH4_REQ13_P15_5  0
#define ERU_CH1_REQ10_P14_3  0
#define ERU_CH5_REQ1_P15_8   0
#define ERU_CH3_REQ6_P02_0   0
#define ERU_CH7_REQ16_P15_1  0

static inline uint8_t exti_flag_get(uint32_t ch) { (void)ch; return 0; }
static inline void exti_flag_clear(uint32_t ch) { (void)ch; }

/* ASCLIN stubs (for ISR error handlers) */
typedef struct { int dummy; } IfxAsclin_Asc;
static IfxAsclin_Asc uart0_handle, uart1_handle, uart2_handle, uart3_handle;
static inline void IfxAsclin_Asc_isrError(IfxAsclin_Asc *h) { (void)h; }

#endif /* ZF_COMMON_HEADFILE_H_ */
