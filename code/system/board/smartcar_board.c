/**
 * @file smartcar_board.c
 * @brief Smart car board startup。
 * @author GS_Mark
 *
 * @par 设计说明
 * 本文件集中完成产品级设备初始化顺序，不向上暴露具体 TC264 引脚或 Vendor API。
 */

#include <stddef.h>

#include "smartcar_board.h"
#include "config.h"
#include "platform/port_if.h"
#include "smartcar_board_resources.h"
#include "motor.h"
#include "servo.h"
#include "input.h"
#include "buzzer.h"

typedef enum
{
    SMARTCAR_BOARD_INIT_UART = 0,
    SMARTCAR_BOARD_INIT_ENCODER,
    SMARTCAR_BOARD_INIT_CALLBACK
} smartcar_board_init_type_t;

typedef void (*smartcar_board_init_callback_t)(void);

typedef struct
{
    mcuio_uart_id_t channel;
    uint32_t baud;
} smartcar_board_uart_init_t;

typedef union
{
    smartcar_board_uart_init_t uart;
    mcuio_encoder_id_t encoder_channel;
    smartcar_board_init_callback_t pf_init;
} smartcar_board_init_param_t;

typedef struct
{
    smartcar_board_init_type_t type;
    smartcar_board_init_param_t param;
} smartcar_board_init_step_t;

static void SmartcarBoard_RunInitStep(const smartcar_board_init_step_t *p_step);
static void SmartcarBoard_RunInitTable(const smartcar_board_init_step_t *p_table,
                                       uint8_t step_count);

static const smartcar_board_init_step_t s_device_init_steps[] =
{
    {
        .type = SMARTCAR_BOARD_INIT_UART,
        .param = { .uart = { .channel = SMARTCAR_UART_CAMERA, .baud = 115200U } },
    },
    {
        .type = SMARTCAR_BOARD_INIT_UART,
        .param = { .uart = { .channel = SMARTCAR_UART_BT, .baud = 115200U } },
    },
    {
        .type = SMARTCAR_BOARD_INIT_ENCODER,
        .param = { .encoder_channel = SMARTCAR_ENCODER_LEFT },
    },
    {
        .type = SMARTCAR_BOARD_INIT_ENCODER,
        .param = { .encoder_channel = SMARTCAR_ENCODER_RIGHT },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Motor_Init },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Servo_Init },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Device_DisplayInit },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Device_CameraInit },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Input_Init },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Buzzer_Init },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Device_ImuInit },
    },
    {
        .type = SMARTCAR_BOARD_INIT_CALLBACK,
        .param = { .pf_init = Device_WirelessInit },
    },
};

static const uint8_t s_device_init_step_count =
    (uint8_t)(sizeof(s_device_init_steps) / sizeof(s_device_init_steps[0]));

/**
 * @brief 执行一个板级设备初始化步骤。
 *
 * Steps:
 *   1. 按步骤类型分发 UART、编码器或无参初始化回调。
 *   2. 忽略空步骤和未知类型，保持启动流程无动态状态。
 *
 * @param[in] p_step 初始化步骤描述。
 * @return void。
 */
static void SmartcarBoard_RunInitStep(const smartcar_board_init_step_t *p_step)
{
    if (p_step == NULL)
    {
        return;
    }

    switch (p_step->type)
    {
        case SMARTCAR_BOARD_INIT_UART:
            McuIo_UartInit(p_step->param.uart.channel,
                           p_step->param.uart.baud);
            break;

        case SMARTCAR_BOARD_INIT_ENCODER:
            McuIo_EncoderInit(p_step->param.encoder_channel);
            break;

        case SMARTCAR_BOARD_INIT_CALLBACK:
            if (p_step->param.pf_init != NULL)
            {
                p_step->param.pf_init();
            }
            break;

        default:
            break;
    }
}

/**
 * @brief 按表顺序执行板级设备初始化步骤。
 *
 * Steps:
 *   1. 从静态初始化表首项开始顺序遍历。
 *   2. 调用单步执行函数完成实际初始化。
 *
 * @param[in] p_table 初始化步骤表。
 * @param[in] step_count 初始化步骤数量。
 * @return void。
 */
static void SmartcarBoard_RunInitTable(const smartcar_board_init_step_t *p_table,
                                       uint8_t step_count)
{
    uint8_t i = 0U;

    if (p_table == NULL)
    {
        return;
    }

    for (i = 0U; i < step_count; i++)
    {
        SmartcarBoard_RunInitStep(&p_table[i]);
    }
}

/**
 * @brief 初始化整车使用的板级设备。
 *
 * Steps:
 *   1. 按静态设备初始化表执行 UART、编码器、执行器和显示设备初始化。
 *   2. 继续按表初始化输入、蜂鸣器、IMU 和无线模块。
 *
 * @return void。
 */
void SmartcarBoard_InitDevices(void)
{
    SmartcarBoard_RunInitTable(s_device_init_steps, s_device_init_step_count);
}

/**
 * @brief 启动周期中断源。
 *
 * Steps:
 *   1. 按 PIT_PERIOD_MS 初始化编码器采样 PIT。
 *   2. 按 PIT_PERIOD_MS 初始化陀螺仪 tick PIT。
 *
 * @return void。
 */
void SmartcarBoard_StartPeriodicIrq(void)
{
    McuIo_PitInit(SMARTCAR_PIT_ENCODER_SAMPLE, PIT_PERIOD_MS);
    McuIo_PitInit(SMARTCAR_PIT_GYRO_TICK, PIT_PERIOD_MS);
}
