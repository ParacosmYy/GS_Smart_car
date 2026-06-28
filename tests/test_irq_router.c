/**
 * @file test_irq_router.c
 * @brief Host-side tests for SmartcarIrqRouter.
 *
 * Compile:
 *   gcc -Itests/stubs -Icode -Icode/scheduler -Icode/platform/system \
 *       -Icode/system/irq tests/test_irq_router.c \
 *       code/system/irq/smartcar_irq_router.c -o test_irq_router.exe
 *
 * The router logic is pure C and independent of TC264 hardware.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Scheduler stubs for host */
#include "scheduler.h"

static uint32_t s_tick_accum = 0U;

void Scheduler_AddTickFromIsr(uint32_t ms)
{
    s_tick_accum += ms;
}

uint32_t Scheduler_GetNowMs(void)
{
    return s_tick_accum;
}

/* Event stubs */
static event_mask_t s_events = 0U;

void event_post_from_isr(event_mask_t events)
{
    s_events |= events;
}

event_mask_t event_get(void)
{
    event_mask_t e = s_events;
    s_events = 0U;
    return e;
}

void event_set_isr(event_mask_t events)
{
    event_post_from_isr(events);
}

/* Include the unit under test */
#include "smartcar_irq_router.h"
#include "irq_fact.h"
#include "irq_port.h"

/* ===== Test helpers ===== */
static irq_fact_t s_handler_result = IRQ_FACT_NONE;

static irq_fact_t test_handler(void)
{
    return s_handler_result;
}

static void reset_state(void)
{
    s_tick_accum = 0U;
    s_events = 0U;
    s_handler_result = IRQ_FACT_NONE;
}

/* ===== Test: FindRoute with exact match ===== */
static void test_find_route_exact(void)
{
    smartcar_irq_route_t routes[2];
    routes[0].source = 0;
    routes[0].handler = test_handler;
    routes[0].fact_mask = IRQ_FACT_GYRO_TICK;
    routes[0].scheduler_events = EVT_GYRO_10MS;
    routes[0].tick_ms = 10U;

    routes[1].source = 1;
    routes[1].handler = test_handler;
    routes[1].fact_mask = IRQ_FACT_CAMERA_FRAME;
    routes[1].scheduler_events = EVT_CAM_FRAME;
    routes[1].tick_ms = 0U;

    SmartcarIrqRouter_Init(routes, 2);

    /* Dispatch source 0 -> should match */
    reset_state();
    s_handler_result = IRQ_FACT_GYRO_TICK;
    SmartcarIrqRouter_Dispatch(0);
    assert(s_events == EVT_GYRO_10MS);
    assert(s_tick_accum == 10U);

    /* Dispatch source 1 -> should match */
    reset_state();
    s_handler_result = IRQ_FACT_CAMERA_FRAME;
    SmartcarIrqRouter_Dispatch(1);
    assert(s_events == EVT_CAM_FRAME);
    assert(s_tick_accum == 0U);

    printf("  PASS test_find_route_exact\n");
}

/* ===== Test: Dispatch with invalid source ===== */
static void test_dispatch_invalid_source(void)
{
    smartcar_irq_route_t routes[1];
    routes[0].source = 0;
    routes[0].handler = test_handler;
    routes[0].fact_mask = IRQ_FACT_NONE;
    routes[0].scheduler_events = EVT_NONE;
    routes[0].tick_ms = 0U;

    SmartcarIrqRouter_Init(routes, 1);

    reset_state();
    SmartcarIrqRouter_Dispatch(99);  /* out of range */
    assert(s_events == 0U);
    assert(s_tick_accum == 0U);

    printf("  PASS test_dispatch_invalid_source\n");
}

/* ===== Test: Fact mask filtering ===== */
static void test_fact_mask_filter(void)
{
    /* Route expects GYRO_TICK, handler returns NONE -> should not publish */
    smartcar_irq_route_t route;
    route.source = 0;
    route.handler = test_handler;
    route.fact_mask = IRQ_FACT_GYRO_TICK;
    route.scheduler_events = EVT_GYRO_10MS;
    route.tick_ms = 10U;

    SmartcarIrqRouter_Init(&route, 1);

    s_handler_result = IRQ_FACT_NONE;
    s_handler_result = IRQ_FACT_NONE;
    reset_state();
    SmartcarIrqRouter_Dispatch(0);
    assert(s_events == 0U);
    assert(s_tick_accum == 0U);

    printf("  PASS test_fact_mask_filter\n");
}

/* ===== Test: NONE fact mask publishes unconditionally ===== */
static void test_none_mask_always_publishes(void)
{
    smartcar_irq_route_t route;
    route.source = 0;
    route.handler = test_handler;
    route.fact_mask = IRQ_FACT_NONE;
    route.scheduler_events = EVT_GYRO_10MS;
    route.tick_ms = 10U;

    SmartcarIrqRouter_Init(&route, 1);

    reset_state();
    s_handler_result = IRQ_FACT_NONE;
    SmartcarIrqRouter_Dispatch(0);
    assert(s_events == EVT_GYRO_10MS);
    assert(s_tick_accum == 10U);

    printf("  PASS test_none_mask_always_publishes\n");
}

int main(void)
{
    printf("=== SmartcarIrqRouter Tests ===\n");
    test_find_route_exact();
    test_dispatch_invalid_source();
    test_fact_mask_filter();
    test_none_mask_always_publishes();
    printf("=== ALL PASS ===\n");
    return 0;
}
