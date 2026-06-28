/**
 * @file platform_tc264.c
 * @brief TC264 平台实现层（包装逐飞 SEEKFREE TC264 库）
 *
 *        本文件集中包装逐飞头文件依赖。少量启动/ISR 入口仍因
 *        TC264 中断宏限制保留 Vendor 头文件。
 *        业务自研代码通过具体 pal_*.h 能力接口访问硬件，
 *        本文件负责将 pal_* 调用转发到逐飞库 API。
 *
 *        换 MCU 时：编写新的 platform_<mcu>.c 替换本文件即可。
 */

#include "pal_camera.h"
#include "pal_comm.h"
#include "pal_display.h"
#include "pal_encoder.h"
#include "pal_gpio.h"
#include "pal_imu.h"
#include "pal_key.h"
#include "pal_pit.h"
#include "pal_pwm.h"
#include "pal_system.h"
#include "pal_uart.h"
#include "zf_common_headfile.h"   /* Vendor 头文件依赖集中在平台/适配层 */

/*===========================================================================
 *  引脚 / 通道 物理映射查找表
 *  换板子时改这里。换 MCU 时改这里 + 改 #include。
 *=========================================================================*/

typedef struct
{
    uint32_t timer;
    uint32_t ch1_pin;
    uint32_t ch2_pin;
} tc264_encoder_map_t;

typedef struct
{
    uint32_t uart;
    uint32_t tx;
    uint32_t rx;
} tc264_uart_map_t;

/* PWM 通道映射 */
static const uint32_t s_pwm_map[PAL_PWM_ID_MAX] =
{
    [PAL_CH_MOTOR_R_FWD]  = ATOM0_CH1_P21_3,   /* 右电机正转 PWM 通道     */
    [PAL_CH_MOTOR_R_REV]  = ATOM0_CH0_P21_2,   /* 右电机反转 PWM 通道     */
    [PAL_CH_MOTOR_L_FWD]  = ATOM0_CH3_P21_5,   /* 左电机正转 PWM 通道     */
    [PAL_CH_MOTOR_L_REV]  = ATOM0_CH2_P21_4,   /* 左电机反转 PWM 通道     */
    [PAL_CH_SERVO]        = ATOM1_CH1_P33_9,   /* 舵机 PWM 输出通道       */
};

/* GPIO 引脚映射 */
static const uint32_t s_gpio_map[PAL_GPIO_ID_MAX] =
{
    [PAL_PIN_BUZZER]      = P11_11,             /* 蜂鸣器 GPIO            */
    [PAL_PIN_KEY1]        = P20_6,              /* 按键 1                 */
    [PAL_PIN_KEY2]        = P20_7,              /* 按键 2                 */
    [PAL_PIN_KEY3]        = P20_8,              /* 按键 3                 */
    [PAL_PIN_KEY4]        = P20_9,              /* 按键 4                 */
    [PAL_PIN_DIP1]        = P15_5,              /* 拨码开关 1             */
    [PAL_PIN_DIP2]        = P15_6,              /* 拨码开关 2             */
    [PAL_PIN_DIP3]        = P15_8,              /* 拨码开关 3             */
    [PAL_PIN_DIP4]        = P15_9,              /* 拨码开关 4             */
};

/* 编码器定时器映射 */
static const tc264_encoder_map_t s_encoder_map[PAL_ENCODER_ID_MAX] =
{
    [PAL_CH_ENCODER_L]    =
    {
        TIM2_ENCODER,
        TIM2_ENCODER_CH1_P33_7,
        TIM2_ENCODER_CH2_P33_6
    },
    [PAL_CH_ENCODER_R]    =
    {
        TIM4_ENCODER,
        TIM4_ENCODER_CH1_P02_8,
        TIM4_ENCODER_CH2_P00_9
    },
};

/* PIT 通道映射 */
static const uint32_t s_pit_map[PAL_PIT_ID_MAX] =
{
    [PAL_CH_PIT_0]        = CCU60_CH0,
    [PAL_CH_PIT_1]        = CCU60_CH1,
    [PAL_CH_PIT_2]        = CCU61_CH0,
    [PAL_CH_PIT_3]        = CCU61_CH1,
};

/* UART 通道映射（tx_pin, rx_pin 成对存储） */
static const tc264_uart_map_t s_uart_map[PAL_UART_ID_MAX] =
{
    [PAL_CH_UART_CAM]     = { UART_1, UART1_TX_P02_2, UART1_RX_P02_3 },
    [PAL_CH_UART_BT]      = { UART_3, UART3_TX_P15_7, UART3_RX_P20_3 },
};

/*===========================================================================
 *  GPIO 实现
 *=========================================================================*/
void pal_gpio_init(pal_gpio_id_t pin, pal_gpio_mode_t mode)
{
    if (pin >= PAL_GPIO_ID_MAX)
    {
        return;
    }

    if (mode == PAL_GPIO_OUTPUT)
    {
        gpio_init(s_gpio_map[pin], GPO, GPIO_LOW, GPO_PUSH_PULL);
    }
    else
    {
        gpio_init(s_gpio_map[pin], GPI, GPIO_HIGH, GPI_PULL_UP);
    }
}

void pal_gpio_high(pal_gpio_id_t pin)
{
    if (pin >= PAL_GPIO_ID_MAX)
    {
        return;
    }

    gpio_high(s_gpio_map[pin]);
}

void pal_gpio_low(pal_gpio_id_t pin)
{
    if (pin >= PAL_GPIO_ID_MAX)
    {
        return;
    }

    gpio_low(s_gpio_map[pin]);
}

uint8_t pal_gpio_read(pal_gpio_id_t pin)
{
    if (pin >= PAL_GPIO_ID_MAX)
    {
        return 0U;
    }

    return (uint8_t)gpio_get_level(s_gpio_map[pin]);
}

/*===========================================================================
 *  PWM 实现
 *=========================================================================*/
void pal_pwm_init(pal_pwm_id_t ch, uint32_t freq_hz, uint32_t duty)
{
    if (ch >= PAL_PWM_ID_MAX)
    {
        return;
    }

    pwm_init(s_pwm_map[ch], freq_hz, duty);
}

void pal_pwm_set_duty(pal_pwm_id_t ch, uint32_t duty)
{
    if (ch >= PAL_PWM_ID_MAX)
    {
        return;
    }

    pwm_set_duty(s_pwm_map[ch], duty);
}

/*===========================================================================
 *  PIT 周期中断实现
 *=========================================================================*/
void pal_pit_init(pal_pit_id_t ch, uint32_t period_ms)
{
    if (ch >= PAL_PIT_ID_MAX)
    {
        return;
    }

    pit_ms_init(s_pit_map[ch], period_ms);
}

void pal_pit_clear_flag(pal_pit_id_t ch)
{
    if (ch >= PAL_PIT_ID_MAX)
    {
        return;
    }

    pit_clear_flag(s_pit_map[ch]);
}

/*===========================================================================
 *  编码器实现
 *=========================================================================*/
void pal_encoder_init(pal_encoder_id_t ch)
{
    if (ch >= PAL_ENCODER_ID_MAX)
    {
        return;
    }

    encoder_dir_init(s_encoder_map[ch].timer,
                     s_encoder_map[ch].ch1_pin,
                     s_encoder_map[ch].ch2_pin);
}

int32_t pal_encoder_get(pal_encoder_id_t ch)
{
    if (ch >= PAL_ENCODER_ID_MAX)
    {
        return 0;
    }

    return encoder_get_count(s_encoder_map[ch].timer);
}

void pal_encoder_clear(pal_encoder_id_t ch)
{
    if (ch >= PAL_ENCODER_ID_MAX)
    {
        return;
    }

    encoder_clear_count(s_encoder_map[ch].timer);
}

/*===========================================================================
 *  UART 实现
 *=========================================================================*/
void pal_uart_init(pal_uart_id_t ch, uint32_t baud)
{
    if (ch >= PAL_UART_ID_MAX)
    {
        return;
    }

    uart_init(s_uart_map[ch].uart, baud, s_uart_map[ch].tx, s_uart_map[ch].rx);
}

/*===========================================================================
 *  摄像头实现
 *=========================================================================*/
void pal_cam_init(void)
{
    mt9v03x_init();
}

bool pal_cam_ready(void)
{
    bool is_ready = false;

    if (mt9v03x_finish_flag == 1)
    {
        is_ready = true;
    }

    return is_ready;
}

void pal_cam_clear(void)
{
    mt9v03x_finish_flag = 0;
}

uint8_t* pal_cam_data(void)
{
    return (uint8_t*)mt9v03x_image;
}

void pal_cam_get_frame_desc(pal_cam_frame_desc_t *p_desc)
{
    if (p_desc == 0)
    {
        return;
    }

    p_desc->width = PAL_CAM_W;
    p_desc->height = PAL_CAM_H;
    p_desc->stride = PAL_CAM_W;
}

/*===========================================================================
 *  陀螺仪实现
 *=========================================================================*/
void pal_gyro_init(void)
{
    icm20602_init();
}

void pal_gyro_read(void)
{
    icm20602_get_gyro();
}

float pal_gyro_z(void)
{
    return icm20602_gyro_transition(icm20602_gyro_z);
}

/*===========================================================================
 *  显示实现
 *=========================================================================*/
void pal_disp_init(void)
{
    tft180_init();
}

void pal_disp_point(int16_t x, int16_t y, uint16_t color)
{
    tft180_draw_point(x, y, color);
}

uint16_t pal_disp_width(void)
{
    return (uint16_t)tft180_width_max;
}

uint16_t pal_disp_height(void)
{
    return (uint16_t)tft180_height_max;
}

void pal_disp_gray(int16_t x, int16_t y, const uint8_t *img,
                   uint16_t w, uint16_t h,
                   uint16_t dis_w, uint16_t dis_h, uint8_t threshold)
{
    tft180_show_gray_image(x, y, img, w, h, dis_w, dis_h, threshold);
}

void pal_disp_str(int16_t x, int16_t y, const char *s)
{
    tft180_show_string(x, y, s);
}

void pal_disp_int(int16_t x, int16_t y, int32_t v, uint8_t digits)
{
    tft180_show_int(x, y, v, digits);
}

/*===========================================================================
 *  无线通信实现
 *=========================================================================*/
void pal_wireless_init(void)
{
    wireless_uart_init();
}

void pal_wireless_rx_handler(void)
{
    wireless_module_uart_handler();
}

void pal_gnss_rx_callback(void)
{
    gnss_uart_callback();
}

/*===========================================================================
 *  按键实现
 *=========================================================================*/
void pal_key_init(uint32_t period_ms)
{
    key_init(period_ms);
}

/*===========================================================================
 *  系统级实现
 *=========================================================================*/
void pal_sys_clock_init(void)
{
    clock_init();
}

void pal_sys_debug_init(void)
{
    debug_init();
}

void pal_sys_core_sync(void)
{
    cpu_wait_event_ready();
}

uint32_t pal_irq_global_disable(void)
{
    return interrupt_global_disable();
}

void pal_irq_global_restore(uint32_t state)
{
    interrupt_global_enable(state);
}

void pal_irq_global_ctrl(uint8_t enable)
{
    interrupt_global_enable(enable);
}
