/**
 * @file ops_tc264.c
 * @brief TC264 目标平台链接期 OPS 端口实现。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件直接实现 SystemPort_*、McuIo_* 和 Device_* 符号，并把产品资源 ID
 * 映射到 TC264 / SEEKFREE / Infineon SDK。切换 MCU 时替换本文件即可保持上层无感。
 */

#include "platform/port_if.h"
#include "smartcar_board_resources.h"
#include "zf_common_headfile.h"

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

/** @brief 产品 PWM 资源到 TC264 ATOM 通道的映射表。 */
static const uint32_t s_tc264_pwm_map[SMARTCAR_PWM_ID_MAX] =
{
    [SMARTCAR_PWM_MOTOR_R_FWD] = ATOM0_CH1_P21_3,
    [SMARTCAR_PWM_MOTOR_R_REV] = ATOM0_CH0_P21_2,
    [SMARTCAR_PWM_MOTOR_L_FWD] = ATOM0_CH3_P21_5,
    [SMARTCAR_PWM_MOTOR_L_REV] = ATOM0_CH2_P21_4,
    [SMARTCAR_PWM_SERVO]       = ATOM1_CH1_P33_9,
};

/** @brief 产品 GPIO 资源到 TC264 端口引脚的映射表。 */
static const uint32_t s_tc264_gpio_map[SMARTCAR_GPIO_ID_MAX] =
{
    [SMARTCAR_GPIO_BUZZER] = P11_11,
    [SMARTCAR_GPIO_KEY1]   = P20_6,
    [SMARTCAR_GPIO_KEY2]   = P20_7,
    [SMARTCAR_GPIO_KEY3]   = P20_8,
    [SMARTCAR_GPIO_KEY4]   = P20_9,
    [SMARTCAR_GPIO_DIP1]   = P15_5,
    [SMARTCAR_GPIO_DIP2]   = P15_6,
    [SMARTCAR_GPIO_DIP3]   = P15_8,
    [SMARTCAR_GPIO_DIP4]   = P15_9,
};

/** @brief 产品编码器资源到 TC264 TIM 编码器通道的映射表。 */
static const tc264_encoder_map_t s_tc264_encoder_map[SMARTCAR_ENCODER_ID_MAX] =
{
    [SMARTCAR_ENCODER_LEFT] =
    {
        TIM2_ENCODER,
        TIM2_ENCODER_CH1_P33_7,
        TIM2_ENCODER_CH2_P33_6,
    },
    [SMARTCAR_ENCODER_RIGHT] =
    {
        TIM4_ENCODER,
        TIM4_ENCODER_CH1_P02_8,
        TIM4_ENCODER_CH2_P00_9,
    },
};

/** @brief 产品 PIT 资源到 TC264 CCU6 通道的映射表。 */
static const uint32_t s_tc264_pit_map[SMARTCAR_PIT_ID_MAX] =
{
    [SMARTCAR_PIT_ENCODER_SAMPLE] = CCU60_CH0,
    [SMARTCAR_PIT_GYRO_TICK]      = CCU60_CH1,
    [SMARTCAR_PIT_SPARE_2]        = CCU61_CH0,
    [SMARTCAR_PIT_SPARE_3]        = CCU61_CH1,
};

/** @brief 产品 UART 资源到 TC264 UART 引脚组合的映射表。 */
static const tc264_uart_map_t s_tc264_uart_map[SMARTCAR_UART_ID_MAX] =
{
    [SMARTCAR_UART_CAMERA] = { UART_1, UART1_TX_P02_2, UART1_RX_P02_3 },
    [SMARTCAR_UART_BT]     = { UART_3, UART3_TX_P15_7, UART3_RX_P20_3 },
};

/**
 * @brief 初始化 TC264 系统时钟。
 *
 * @return void。
 */
void SystemPort_ClockInit(void)
{
    clock_init();
}

/**
 * @brief 初始化调试串口。
 *
 * @return void。
 */
void SystemPort_DebugInit(void)
{
    debug_init();
}

/**
 * @brief 等待多核启动同步。
 *
 * @return void。
 */
void SystemPort_CoreSync(void)
{
    cpu_wait_event_ready();
}

/**
 * @brief 关闭全局中断并返回恢复状态。
 *
 * @return Vendor 全局中断恢复状态。
 */
uint32_t SystemPort_IrqGlobalDisable(void)
{
    return interrupt_global_disable();
}

/**
 * @brief 恢复全局中断状态。
 *
 * @param[in] state SystemPort_IrqGlobalDisable 返回的状态。
 * @return void。
 */
void SystemPort_IrqGlobalRestore(uint32_t state)
{
    interrupt_global_enable(state);
}

/**
 * @brief 按 Vendor 恢复参数控制全局中断。
 *
 * @param[in] restore_state TC264 Vendor 恢复参数；0U 表示打开全局中断，非 0U 保持关闭状态。
 * @return void。
 */
void SystemPort_IrqGlobalCtrl(uint8_t restore_state)
{
    interrupt_global_enable(restore_state);
}

/**
 * @brief 初始化 GPIO。
 *
 * Steps:
 *   1. 校验产品 GPIO 资源 ID。
 *   2. 按输出或输入模式调用 TC264 GPIO 初始化。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @param[in] mode MCUIO_GPIO_OUTPUT 或输入模式。
 * @return void。
 */
void McuIo_GpioInit(mcuio_gpio_id_t pin, uint8_t mode)
{
    if (pin >= SMARTCAR_GPIO_ID_MAX)
    {
        return;
    }

    if (mode == MCUIO_GPIO_OUTPUT)
    {
        gpio_init(s_tc264_gpio_map[pin], GPO, GPIO_LOW, GPO_PUSH_PULL);
    }
    else
    {
        gpio_init(s_tc264_gpio_map[pin], GPI, GPIO_HIGH, GPI_PULL_UP);
    }
}

/**
 * @brief 拉高 GPIO。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @return void。
 */
void McuIo_GpioHigh(mcuio_gpio_id_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_high(s_tc264_gpio_map[pin]);
    }
}

/**
 * @brief 拉低 GPIO。
 *
 * @param[in] pin 产品 GPIO 资源 ID。
 * @return void。
 */
void McuIo_GpioLow(mcuio_gpio_id_t pin)
{
    if (pin < SMARTCAR_GPIO_ID_MAX)
    {
        gpio_low(s_tc264_gpio_map[pin]);
    }
}

/**
 * @brief 初始化 PWM 通道。
 *
 * @param[in] channel 产品 PWM 资源 ID。
 * @param[in] freq_hz PWM 频率，单位 Hz。
 * @param[in] duty 初始占空比。
 * @return void。
 */
void McuIo_PwmInit(mcuio_pwm_id_t channel, uint32_t freq_hz, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_init(s_tc264_pwm_map[channel], freq_hz, duty);
    }
}

/**
 * @brief 设置 PWM 占空比。
 *
 * @param[in] channel 产品 PWM 资源 ID。
 * @param[in] duty 目标占空比。
 * @return void。
 */
void McuIo_PwmSetDuty(mcuio_pwm_id_t channel, uint32_t duty)
{
    if (channel < SMARTCAR_PWM_ID_MAX)
    {
        pwm_set_duty(s_tc264_pwm_map[channel], duty);
    }
}

/**
 * @brief 初始化 UART。
 *
 * Steps:
 *   1. 校验产品 UART 资源 ID。
 *   2. 从映射表取出 UART 编号和 TX/RX 引脚。
 *   3. 调用 Vendor UART 初始化。
 *
 * @param[in] channel 产品 UART 资源 ID。
 * @param[in] baud 波特率。
 * @return void。
 */
void McuIo_UartInit(mcuio_uart_id_t channel, uint32_t baud)
{
    if (channel >= SMARTCAR_UART_ID_MAX)
    {
        return;
    }

    uart_init(s_tc264_uart_map[channel].uart,
              baud,
              s_tc264_uart_map[channel].tx,
              s_tc264_uart_map[channel].rx);
}

/**
 * @brief 初始化编码器通道。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return void。
 */
void McuIo_EncoderInit(mcuio_encoder_id_t channel)
{
    if (channel >= SMARTCAR_ENCODER_ID_MAX)
    {
        return;
    }

    encoder_dir_init(s_tc264_encoder_map[channel].timer,
                     s_tc264_encoder_map[channel].ch1_pin,
                     s_tc264_encoder_map[channel].ch2_pin);
}

/**
 * @brief 读取编码器计数。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return 编码器计数；资源 ID 无效时返回 0。
 */
int32_t McuIo_EncoderGet(mcuio_encoder_id_t channel)
{
    int32_t count = 0;

    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        count = encoder_get_count(s_tc264_encoder_map[channel].timer);
    }

    return count;
}

/**
 * @brief 清零编码器计数。
 *
 * @param[in] channel 产品编码器资源 ID。
 * @return void。
 */
void McuIo_EncoderClear(mcuio_encoder_id_t channel)
{
    if (channel < SMARTCAR_ENCODER_ID_MAX)
    {
        encoder_clear_count(s_tc264_encoder_map[channel].timer);
    }
}

/**
 * @brief 初始化 PIT 周期中断。
 *
 * @param[in] channel 产品 PIT 资源 ID。
 * @param[in] period_ms 周期，单位 ms。
 * @return void。
 */
void McuIo_PitInit(mcuio_pit_id_t channel, uint32_t period_ms)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_ms_init(s_tc264_pit_map[channel], period_ms);
    }
}

/**
 * @brief 清除 PIT 中断标志。
 *
 * @param[in] channel 产品 PIT 资源 ID。
 * @return void。
 */
void McuIo_PitClearFlag(mcuio_pit_id_t channel)
{
    if (channel < SMARTCAR_PIT_ID_MAX)
    {
        pit_clear_flag(s_tc264_pit_map[channel]);
    }
}

/**
 * @brief 查询 TC264 摄像头帧完成标志。
 *
 * @return true 表示有新帧；false 表示暂无新帧。
 */
static bool Tc264Camera_IsFrameReady(void)
{
    return (mt9v03x_finish_flag == 1);
}

/**
 * @brief 清除 TC264 摄像头帧完成标志。
 *
 * @return void。
 */
static void Tc264Camera_ClearFrameReady(void)
{
    mt9v03x_finish_flag = 0;
}

/**
 * @brief 获取 TC264 摄像头灰度图像缓冲区。
 *
 * @return Vendor 摄像头图像缓冲区首地址。
 */
static uint8_t *Tc264Camera_GetFrameData(void)
{
    return (uint8_t *)mt9v03x_image;
}

/**
 * @brief 初始化 MT9V03X 摄像头。
 *
 * @return void。
 */
void Device_CameraInit(void)
{
    mt9v03x_init();
}

/**
 * @brief 查询摄像头帧是否采集完成。
 *
 * @return true 表示有新帧；false 表示暂无新帧。
 */
bool Device_CameraReady(void)
{
    return Tc264Camera_IsFrameReady();
}

/**
 * @brief 清除摄像头帧完成标志。
 *
 * @return void。
 */
void Device_CameraClear(void)
{
    Tc264Camera_ClearFrameReady();
}

/**
 * @brief 获取摄像头灰度图像缓冲区。
 *
 * @return 摄像头图像缓冲区首地址。
 */
uint8_t *Device_CameraData(void)
{
    return Tc264Camera_GetFrameData();
}

/**
 * @brief 获取摄像头帧几何信息。
 *
 * @param[out] p_desc 帧描述输出指针。
 * @return void。
 */
void Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc)
{
    if (p_desc != 0)
    {
        p_desc->width = (uint16_t)MT9V03X_W;
        p_desc->height = (uint16_t)MT9V03X_H;
        p_desc->stride = (uint16_t)MT9V03X_W;
    }
}

/**
 * @brief 初始化 TFT180 调试显示屏。
 *
 * @return void。
 */
void Device_DisplayInit(void)
{
    tft180_init();
}

/**
 * @brief 绘制单个屏幕像素。
 *
 * @param[in] x X 坐标。
 * @param[in] y Y 坐标。
 * @param[in] color RGB565 颜色。
 * @return void。
 */
void Device_DisplayPoint(int16_t x, int16_t y, uint16_t color)
{
    tft180_draw_point(x, y, color);
}

/**
 * @brief 显示灰度图像。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] p_img 灰度图像缓冲区。
 * @param[in] w 原图宽度。
 * @param[in] h 原图高度。
 * @param[in] dis_w 显示宽度。
 * @param[in] dis_h 显示高度。
 * @param[in] threshold 显示阈值。
 * @return void。
 */
void Device_DisplayGray(int16_t x,
                        int16_t y,
                        const uint8_t *p_img,
                        uint16_t w,
                        uint16_t h,
                        uint16_t dis_w,
                        uint16_t dis_h,
                        uint8_t threshold)
{
    tft180_show_gray_image(x, y, p_img, w, h, dis_w, dis_h, threshold);
}

/**
 * @brief 显示字符串。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] p_str 字符串指针。
 * @return void。
 */
void Device_DisplayStr(int16_t x, int16_t y, const char *p_str)
{
    tft180_show_string(x, y, p_str);
}

/**
 * @brief 显示整数。
 *
 * @param[in] x 显示起点 X 坐标。
 * @param[in] y 显示起点 Y 坐标。
 * @param[in] value 待显示整数。
 * @param[in] digits 显示位数。
 * @return void。
 */
void Device_DisplayInt(int16_t x, int16_t y, int32_t value, uint8_t digits)
{
    tft180_show_int(x, y, value, digits);
}

/**
 * @brief 初始化 ICM20602 IMU。
 *
 * @return void。
 */
void Device_ImuInit(void)
{
    icm20602_init();
}

/**
 * @brief 读取 IMU 陀螺仪原始数据。
 *
 * @return void。
 */
void Device_ImuRead(void)
{
    icm20602_get_gyro();
}

/**
 * @brief 获取 Z 轴角速度工程值。
 *
 * @return Z 轴角速度。
 */
float Device_ImuZ(void)
{
    return icm20602_gyro_transition(icm20602_gyro_z);
}

/**
 * @brief 初始化无线串口模块。
 *
 * @return void。
 */
void Device_WirelessInit(void)
{
    wireless_uart_init();
}
