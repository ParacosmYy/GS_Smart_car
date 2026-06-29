#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "event.h"
#include "platform/interface/device_if.h"
#include "scheduler.h"
#include "system/irq/smartcar_irq_router.h"
#include "vision.h"

static uint8_t s_camera_frame[VISION_RAW_H][VISION_RAW_W];
static uint8_t s_failures = 0U;
static uint8_t s_event_task_count = 0U;
static uint8_t s_periodic_task_count = 0U;

static void check_u32(const char *p_name, uint32_t actual, uint32_t expected)
{
    if (actual != expected)
    {
        printf("FAIL %s: actual=%lu expected=%lu\n",
               p_name,
               (unsigned long)actual,
               (unsigned long)expected);
        s_failures++;
    }
}

static void check_true(const char *p_name, uint8_t condition)
{
    if (condition == 0U)
    {
        printf("FAIL %s\n", p_name);
        s_failures++;
    }
}

uint32_t SystemPort_IrqGlobalDisable(void)
{
    return 0U;
}

void SystemPort_IrqGlobalRestore(uint32_t state)
{
    (void)state;
}

void SystemPort_ClockInit(void)
{
}

void SystemPort_DebugInit(void)
{
}

void SystemPort_CoreSync(void)
{
}

void SystemPort_IrqGlobalCtrl(uint8_t state)
{
    (void)state;
}

void Device_CameraInit(void)
{
}

bool Device_CameraReady(void)
{
    return true;
}

void Device_CameraClear(void)
{
}

uint8_t *Device_CameraData(void)
{
    return &s_camera_frame[0][0];
}

void Device_CameraGetFrameDesc(camera_frame_desc_t *p_desc)
{
    if (p_desc != 0)
    {
        p_desc->width = VISION_RAW_W;
        p_desc->height = VISION_RAW_H;
        p_desc->stride = VISION_RAW_W;
    }
}

static irq_fact_t smoke_camera_irq(void)
{
    return IRQ_FACT_CAMERA_FRAME;
}

static irq_fact_t smoke_gyro_irq(void)
{
    return IRQ_FACT_GYRO_TICK;
}

static const target_irq_route_t s_smoke_routes[] =
{
    {SMARTCAR_IRQ_SOURCE_DMA_CH5, smoke_camera_irq, IRQ_FACT_CAMERA_FRAME, EVT_CAM_FRAME, 0U},
    {SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1, smoke_gyro_irq, IRQ_FACT_GYRO_TICK, EVT_GYRO_10MS, 10U},
};

const target_irq_route_t *TargetPlatform_GetIrqRoutes(uint16_t *p_count)
{
    if (p_count != 0)
    {
        *p_count = (uint16_t)(sizeof(s_smoke_routes) / sizeof(s_smoke_routes[0]));
    }

    return s_smoke_routes;
}

static void smoke_event_task(event_mask_t events)
{
    if ((events & EVT_CAM_FRAME) != 0U)
    {
        s_event_task_count++;
    }
}

static void smoke_periodic_task(event_mask_t events)
{
    (void)events;
    s_periodic_task_count++;
}

static void test_event(void)
{
    event_post_from_isr(EVT_CAM_FRAME);
    check_u32("event frame", event_get(), EVT_CAM_FRAME);
    check_u32("event empty", event_get(), EVT_NONE);

    event_post_from_isr(EVT_GYRO_10MS);
    event_post_from_isr(EVT_GYRO_10MS);
    check_u32("gyro first", event_get(), EVT_GYRO_10MS);
    check_u32("gyro second", event_get(), EVT_GYRO_10MS);
    check_u32("gyro empty", event_get(), EVT_NONE);
}

static void test_scheduler(void)
{
    scheduler_init();
    s_event_task_count = 0U;
    s_periodic_task_count = 0U;

    check_true("scheduler event add", scheduler_add(smoke_event_task, 0U, EVT_CAM_FRAME) >= 0);
    check_true("scheduler periodic add", scheduler_add(smoke_periodic_task, 10U, EVT_NONE) >= 0);

    event_post_from_isr(EVT_CAM_FRAME);
    scheduler_run();
    check_u32("scheduler event task", s_event_task_count, 1U);
    check_u32("scheduler periodic cold", s_periodic_task_count, 0U);

    Scheduler_AddTickFromIsr(10U);
    scheduler_run();
    check_u32("scheduler periodic task", s_periodic_task_count, 1U);
}

static void test_irq_router(void)
{
    scheduler_init();
    (void)event_get();

    SmartcarIrq_Dispatch(SMARTCAR_IRQ_SOURCE_DMA_CH5);
    check_u32("irq camera event", event_get(), EVT_CAM_FRAME);

    SmartcarIrq_Dispatch(SMARTCAR_IRQ_SOURCE_CCU60_PIT_CH1);
    check_u32("irq gyro event", event_get(), EVT_GYRO_10MS);
    check_u32("irq gyro tick", Scheduler_GetNowMs(), 10U);
}

static void fill_straight_track(void)
{
    uint16_t row;
    uint16_t col;

    memset(s_camera_frame, 255, sizeof(s_camera_frame));

    for (row = 0U; row < VISION_RAW_H; row++)
    {
        for (col = 70U; col < 118U; col++)
        {
            s_camera_frame[row][col] = 0U;
        }
    }
}

static void test_vision(void)
{
    vision_control_snapshot_t control = {0};
    vision_debug_snapshot_t debug = {0};

    fill_straight_track();
    Vision_Process();
    Vision_GetControlSnapshot(&control);
    Vision_GetDebugSnapshot(&debug);

    check_true("vision debug buffer", debug.p_binary_zip != 0);
    check_u32("vision line count", debug.line_count, VISION_ZIP_IMAGE_H);
    check_true("vision mid range", (control.mid_line >= 40U) && (control.mid_line <= 54U));
    check_true("vision error range", (control.calculate_error >= -8) && (control.calculate_error <= 8));
    check_true("vision lost count", control.lost_count <= 2U);
}

int main(void)
{
    test_event();
    test_scheduler();
    test_irq_router();
    test_vision();

    if (s_failures != 0U)
    {
        printf("smoke-failures=%u\n", s_failures);
        return 1;
    }

    printf("smoke-pass\n");
    return 0;
}
