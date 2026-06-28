/**
 * @file platform_tc264.c
 * @brief TC264 平台实现层（包装逐飞 SEEKFREE TC264 库）
 *
 *        本文件集中包装逐飞头文件依赖。少量启动/ISR 入口仍因
 *        TC264 中断宏限制保留 Vendor 头文件。
 *        业务自研代码通过 platform.h 的 pal_* 接口访问硬件，
 *        本文件负责将 pal_* 调用转发到逐飞库 API。
 *
 *        换 MCU 时：编写新的 platform_<mcu>.c 替换本文件即可。
 */

#include "platform.h"
#include "zf_common_headfile.h"   /* ← 全项目唯一的逐飞头文件依赖 */

/*===========================================================================
 *  引脚 / 通道 物理映射查找表
 *  换板子时改这里。换 MCU 时改这里 + 改 #include。
 *=========================================================================*/

/* GPIO 引脚映射 */
static const uint32_t s_pin_map[] =
{
    [PAL_CH_MOTOR_R_FWD]  = ATOM0_CH1_P21_3,   /* 右电机正转 PWM 通道     */
    [PAL_CH_MOTOR_R_REV]  = ATOM0_CH0_P21_2,   /* 右电机反转 PWM 通道     */
    [PAL_CH_MOTOR_L_FWD]  = ATOM0_CH3_P21_5,   /* 左电机正转 PWM 通道     */
    [PAL_CH_MOTOR_L_REV]  = ATOM0_CH2_P21_4,   /* 左电机反转 PWM 通道     */
    [PAL_CH_SERVO]        = ATOM1_CH1_P33_9,   /* 舵机 PWM 输出通道       */
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
static const uint32_t s_encoder_map[] =
{
    [PAL_CH_ENCODER_L]    = TIM2_ENCODER,
    [PAL_CH_ENCODER_R]    = TIM4_ENCODER,
};

/* PIT 通道映射 */
static const uint32_t s_pit_map[] =
{
    [PAL_CH_PIT_0]        = CCU60_CH0,
    [PAL_CH_PIT_1]        = CCU60_CH1,
    [PAL_CH_PIT_2]        = CCU61_CH0,
    [PAL_CH_PIT_3]        = CCU61_CH1,
};

/* UART 通道映射（tx_pin, rx_pin 成对存储） */
static const struct { uint32_t uart; uint32_t tx; uint32_t rx; } s_uart_map[] =
{
    [PAL_CH_UART_CAM]     = { UART_1, UART1_TX_P02_2, UART1_RX_P02_3 },
    [PAL_CH_UART_BT]      = { UART_3, UART3_TX_P15_7, UART3_RX_P20_3 },
};

/*===========================================================================
 *  GPIO 实现
 *=========================================================================*/
void pal_gpio_init(pal_ch_t pin, pal_gpio_mode_t mode)
{
    if (mode == PAL_GPIO_OUTPUT)
    {
        gpio_init(s_pin_map[pin], GPO, GPIO_LOW, GPO_PUSH_PULL);
    }
    else
    {
        gpio_init(s_pin_map[pin], GPI, GPIO_HIGH, GPI_PULL_UP);
    }
}

void pal_gpio_high(pal_ch_t pin)
{
    gpio_high(s_pin_map[pin]);
}

void pal_gpio_low(pal_ch_t pin)
{
    gpio_low(s_pin_map[pin]);
}

uint8_t pal_gpio_read(pal_ch_t pin)
{
    return (uint8_t)gpio_get_level(s_pin_map[pin]);
}

/*===========================================================================
 *  PWM 实现
 *=========================================================================*/
void pal_pwm_init(pal_ch_t ch, uint32_t freq_hz, uint32_t duty)
{
    pwm_init(s_pin_map[ch], freq_hz, duty);
}

void pal_pwm_set_duty(pal_ch_t ch, uint32_t duty)
{
    pwm_set_duty(s_pin_map[ch], duty);
}

/*===========================================================================
 *  PIT 周期中断实现
 *=========================================================================*/
void pal_pit_init(pal_ch_t ch, uint32_t period_ms)
{
    pit_ms_init(s_pit_map[ch], period_ms);
}

void pal_pit_clear_flag(pal_ch_t ch)
{
    pit_clear_flag(s_pit_map[ch]);
}

/*===========================================================================
 *  编码器实现
 *=========================================================================*/
void pal_encoder_init(pal_ch_t ch)
{
    if (ch == PAL_CH_ENCODER_L)
    {
        encoder_dir_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P33_7, TIM2_ENCODER_CH2_P33_6);
    }
    else
    {
        encoder_dir_init(TIM4_ENCODER, TIM4_ENCODER_CH1_P02_8, TIM4_ENCODER_CH2_P00_9);
    }
}

int32_t pal_encoder_get(pal_ch_t ch)
{
    return encoder_get_count(s_encoder_map[ch]);
}

void pal_encoder_clear(pal_ch_t ch)
{
    encoder_clear_count(s_encoder_map[ch]);
}

/*===========================================================================
 *  UART 实现
 *=========================================================================*/
void pal_uart_init(pal_ch_t ch, uint32_t baud)
{
    uart_init(s_uart_map[ch].uart, baud, s_uart_map[ch].tx, s_uart_map[ch].rx);
}

/*===========================================================================
 *  摄像头（MT9V03X）实现
 *=========================================================================*/
void pal_cam_init(void)
{
    mt9v03x_init();
}

bool pal_cam_ready(void)
{
    return (mt9v03x_finish_flag == 1) ? true : false;
}

void pal_cam_clear(void)
{
    mt9v03x_finish_flag = 0;
}

uint8_t* pal_cam_data(void)
{
    return (uint8_t*)mt9v03x_image;
}

/*===========================================================================
 *  陀螺仪（ICM20602）实现
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
 *  显示（TFT180）实现
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
