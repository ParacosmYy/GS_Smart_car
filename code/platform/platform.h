/**
 * @file platform.h
 * @brief Hardware Platform Abstraction Layer.
 *
 *        This file defines platform-neutral hardware interfaces. Self-owned
 *        code in BSP/Service/App/Common depends on this header instead of
 *        directly calling a chip SDK such as SEEKFREE or STM32 HAL.
 *
 *        MCU porting flow:
 *          1. Implement a new platform_<mcu>.c wrapper for the target SDK.
 *          2. Replace the platform_<mcu>.c source file in the build project.
 *          3. Keep App/Service/BSP/Common code unchanged.
 *
 *        Dependency direction:
 *          App → Service → BSP → platform.h（本文件）
 *                                  ↓
 *                          platform_<mcu>.c（实现层）
 *                                  ↓
 *                              Vendor SDK
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>

/*===========================================================================
 *  平台资源逻辑编号
 *  平台无关。物理映射在 platform_<mcu>.c 的查找表中。
 *=========================================================================*/
typedef enum
{
    PAL_CH_MOTOR_R_FWD  = 0,   /* 右电机正转 PWM             */
    PAL_CH_MOTOR_R_REV,        /* 右电机反转 PWM             */
    PAL_CH_MOTOR_L_FWD,        /* 左电机正转 PWM             */
    PAL_CH_MOTOR_L_REV,        /* 左电机反转 PWM             */
    PAL_CH_SERVO,              /* 舵机 PWM 输出               */
    PAL_PWM_ID_MAX
} pal_pwm_id_t;

typedef enum
{
    PAL_PIN_BUZZER = 0,        /* 蜂鸣器控制引脚              */
    PAL_PIN_KEY1,              /* 按键 1                     */
    PAL_PIN_KEY2,              /* 按键 2                     */
    PAL_PIN_KEY3,              /* 按键 3                     */
    PAL_PIN_KEY4,              /* 按键 4                     */
    PAL_PIN_DIP1,              /* 拨码开关 1                 */
    PAL_PIN_DIP2,              /* 拨码开关 2                 */
    PAL_PIN_DIP3,              /* 拨码开关 3                 */
    PAL_PIN_DIP4,              /* 拨码开关 4                 */
    PAL_GPIO_ID_MAX
} pal_gpio_id_t;

typedef enum
{
    PAL_CH_ENCODER_L = 0,      /* 左编码器定时器              */
    PAL_CH_ENCODER_R,          /* 右编码器定时器              */
    PAL_ENCODER_ID_MAX
} pal_encoder_id_t;

typedef enum
{
    PAL_CH_PIT_0 = 0,          /* 周期中断通道 0（编码器采样）*/
    PAL_CH_PIT_1,              /* 周期中断通道 1（陀螺仪采样）*/
    PAL_CH_PIT_2,              /* 周期中断通道 2（预留扩展）  */
    PAL_CH_PIT_3,              /* 周期中断通道 3（预留扩展）  */
    PAL_PIT_ID_MAX
} pal_pit_id_t;

typedef enum
{
    PAL_CH_UART_CAM = 0,       /* 摄像头配置 UART            */
    PAL_CH_UART_BT,            /* 蓝牙/无线 UART             */
    PAL_UART_ID_MAX
} pal_uart_id_t;

/*===========================================================================
 *  GPIO 抽象
 *=========================================================================*/
typedef enum
{
    PAL_GPIO_OUTPUT,           /* 推挽输出                    */
    PAL_GPIO_INPUT             /* 上拉输入                    */
} pal_gpio_mode_t;

void     pal_gpio_init(pal_gpio_id_t pin, pal_gpio_mode_t mode);
void     pal_gpio_high(pal_gpio_id_t pin);
void     pal_gpio_low (pal_gpio_id_t pin);
uint8_t  pal_gpio_read(pal_gpio_id_t pin);

/*===========================================================================
 *  PWM 抽象
 *=========================================================================*/
void pal_pwm_init   (pal_pwm_id_t ch, uint32_t freq_hz, uint32_t duty);
void pal_pwm_set_duty(pal_pwm_id_t ch, uint32_t duty);

/*===========================================================================
 *  周期中断（PIT）抽象
 *=========================================================================*/
void pal_pit_init      (pal_pit_id_t ch, uint32_t period_ms);
void pal_pit_clear_flag(pal_pit_id_t ch);

/*===========================================================================
 *  编码器抽象
 *=========================================================================*/
void    pal_encoder_init (pal_encoder_id_t ch);
int32_t pal_encoder_get  (pal_encoder_id_t ch);
void    pal_encoder_clear(pal_encoder_id_t ch);
void    pal_encoder_take_snapshot(int *p_left_sum, int *p_right_sum, int *p_sample_count);

/*===========================================================================
 *  UART 抽象
 *=========================================================================*/
void pal_uart_init(pal_uart_id_t ch, uint32_t baud);

/*===========================================================================
 *  摄像头抽象
 *=========================================================================*/
#define PAL_CAM_W  188               /* 图像宽度（像素）           */
#define PAL_CAM_H  120               /* 图像高度（像素）           */

typedef struct
{
    uint16_t width;                  /* 图像有效宽度               */
    uint16_t height;                 /* 图像有效高度               */
    uint16_t stride;                 /* 相邻两行首地址间隔         */
} pal_cam_frame_desc_t;

void     pal_cam_init  (void);       /* 初始化摄像头采集           */
bool     pal_cam_ready (void);       /* 新一帧是否就绪             */
void     pal_cam_clear (void);       /* 清除帧就绪标志             */
uint8_t* pal_cam_data  (void);       /* 返回图像数据首地址（PAL_CAM_H × PAL_CAM_W）*/
void     pal_cam_get_frame_desc(pal_cam_frame_desc_t *p_desc);

/*===========================================================================
 *  陀螺仪抽象
 *=========================================================================*/
void  pal_gyro_init (void);          /* 初始化陀螺仪               */
void  pal_gyro_read (void);          /* 触发一次 SPI 读取          */
float pal_gyro_z    (void);          /* 返回 Z 轴角速度（已转换为 deg/s）*/

/*===========================================================================
 *  显示抽象
 *=========================================================================*/
void pal_disp_init(void);
void pal_disp_point(int16_t x, int16_t y, uint16_t color);
uint16_t pal_disp_width(void);
uint16_t pal_disp_height(void);
void pal_disp_gray(int16_t x, int16_t y, const uint8_t *img,
                   uint16_t w, uint16_t h,
                   uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void pal_disp_str (int16_t x, int16_t y, const char *s);
void pal_disp_int (int16_t x, int16_t y, int32_t v, uint8_t digits);

/*===========================================================================
 *  无线通信抽象
 *=========================================================================*/
void pal_wireless_init(void);
void pal_wireless_rx_handler(void); /* 无线模块 UART RX 中断回调  */
void pal_gnss_rx_callback(void);    /* GNSS/蓝牙 UART RX 中断回调 */

/*===========================================================================
 *  按键抽象
 *=========================================================================*/
void pal_key_init(uint32_t period_ms);

/*===========================================================================
 *  系统级抽象
 *=========================================================================*/
void pal_sys_clock_init (void);      /* 系统时钟初始化             */
void pal_sys_debug_init (void);      /* 调试串口初始化             */
void pal_sys_core_sync  (void);      /* 多核同步等待               */
uint32_t pal_irq_global_disable(void);       /* 关闭全局中断并返回原状态 */
void     pal_irq_global_restore(uint32_t state); /* 按原状态恢复全局中断 */
void     pal_irq_global_ctrl(uint8_t state); /* 兼容旧接口：state=0 时使能中断 */

/*===========================================================================
 *  兼容性定义（供历史代码平滑迁移）
 *=========================================================================*/
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* PLATFORM_H_ */
