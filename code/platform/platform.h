/**
 * @file platform.h
 * @brief 硬件平台抽象层（Platform Abstraction Layer）
 *
 *        本文件定义平台无关的硬件接口。所有自研代码（BSP/Service/App/Common）
 *        只依赖此头文件，不直接调用任何芯片 SDK（逐飞库 / STM32 HAL 等）。
 *
 *        换 MCU 时：
 *          1. 编写新的 platform_<mcu>.c（如 platform_stm32.c）包装目标 SDK
 *          2. 替换编译工程中的 platform_<mcu>.c 源文件
 *          3. 其余代码零改动
 *
 *        依赖方向：
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
 *  引脚 / 通道逻辑编号
 *  平台无关。物理映射在 platform_<mcu>.c 的查找表中。
 *=========================================================================*/
typedef enum
{
    /* --- 电机 PWM 通道（H 桥：每轮正转/反转各一路 PWM）--- */
    PAL_CH_MOTOR_R_FWD  = 0,   /* 右电机正转 PWM             */
    PAL_CH_MOTOR_R_REV,        /* 右电机反转 PWM             */
    PAL_CH_MOTOR_L_FWD,        /* 左电机正转 PWM             */
    PAL_CH_MOTOR_L_REV,        /* 左电机反转 PWM             */
    PAL_CH_SERVO,              /* 舵机 PWM 输出               */

    /* --- GPIO 引脚 --- */
    PAL_PIN_BUZZER,            /* 蜂鸣器控制引脚              */
    PAL_PIN_KEY1,              /* 按键 1                     */
    PAL_PIN_KEY2,              /* 按键 2                     */
    PAL_PIN_KEY3,              /* 按键 3                     */
    PAL_PIN_KEY4,              /* 按键 4                     */
    PAL_PIN_DIP1,              /* 拨码开关 1                 */
    PAL_PIN_DIP2,              /* 拨码开关 2                 */
    PAL_PIN_DIP3,              /* 拨码开关 3                 */
    PAL_PIN_DIP4,              /* 拨码开关 4                 */

    /* --- 外设通道 --- */
    PAL_CH_ENCODER_L,          /* 左编码器定时器              */
    PAL_CH_ENCODER_R,          /* 右编码器定时器              */
    PAL_CH_PIT_0,              /* 周期中断通道 0（编码器采样）*/
    PAL_CH_PIT_1,              /* 周期中断通道 1（陀螺仪采样）*/
    PAL_CH_UART_CAM,           /* 摄像头配置 UART            */
    PAL_CH_UART_BT,            /* 蓝牙/无线 UART             */

    PAL_CH_MAX                 /* 哨兵值                      */
} pal_ch_t;

/*===========================================================================
 *  GPIO 抽象
 *=========================================================================*/
typedef enum
{
    PAL_GPIO_OUTPUT,           /* 推挽输出                    */
    PAL_GPIO_INPUT             /* 上拉输入                    */
} pal_gpio_mode_t;

void     pal_gpio_init(pal_ch_t pin, pal_gpio_mode_t mode);
void     pal_gpio_high(pal_ch_t pin);
void     pal_gpio_low (pal_ch_t pin);
uint8_t  pal_gpio_read(pal_ch_t pin);

/*===========================================================================
 *  PWM 抽象
 *=========================================================================*/
void pal_pwm_init   (pal_ch_t ch, uint32_t freq_hz, uint32_t duty);
void pal_pwm_set_duty(pal_ch_t ch, uint32_t duty);

/*===========================================================================
 *  周期中断（PIT）抽象
 *=========================================================================*/
void pal_pit_init      (pal_ch_t ch, uint32_t period_ms);
void pal_pit_clear_flag(pal_ch_t ch);

/*===========================================================================
 *  编码器抽象
 *=========================================================================*/
void    pal_encoder_init (pal_ch_t ch);
int32_t pal_encoder_get  (pal_ch_t ch);
void    pal_encoder_clear(pal_ch_t ch);

/*===========================================================================
 *  UART 抽象
 *=========================================================================*/
void pal_uart_init(pal_ch_t ch, uint32_t baud);

/*===========================================================================
 *  摄像头（MT9V03X）抽象
 *=========================================================================*/
#define PAL_CAM_W  188               /* 图像宽度（像素）           */
#define PAL_CAM_H  120               /* 图像高度（像素）           */

void     pal_cam_init  (void);       /* 初始化摄像头采集           */
bool     pal_cam_ready (void);       /* 新一帧是否就绪             */
void     pal_cam_clear (void);       /* 清除帧就绪标志             */
uint8_t* pal_cam_data  (void);       /* 返回图像数据首地址（PAL_CAM_H × PAL_CAM_W）*/

/*===========================================================================
 *  陀螺仪（ICM20602）抽象
 *=========================================================================*/
void  pal_gyro_init (void);          /* 初始化陀螺仪               */
void  pal_gyro_read (void);          /* 触发一次 SPI 读取          */
float pal_gyro_z    (void);          /* 返回 Z 轴角速度（已转换为 deg/s）*/

/*===========================================================================
 *  显示（TFT180）抽象
 *=========================================================================*/
void pal_disp_init(void);
void pal_disp_gray(int16_t x, int16_t y, const uint8_t *img,
                   uint16_t w, uint16_t h,
                   uint16_t dis_w, uint16_t dis_h, uint8_t threshold);
void pal_disp_str (int16_t x, int16_t y, const char *s);
void pal_disp_int (int16_t x, int16_t y, int32_t v, uint8_t digits);

/*===========================================================================
 *  无线通信抽象
 *=========================================================================*/
void pal_wireless_init(void);

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
void pal_irq_global_ctrl(uint8_t enable); /* 全局中断控制（0=关/1=开）*/

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
