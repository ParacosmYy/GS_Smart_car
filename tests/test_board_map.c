/**
 * @file test_board_map.c
 * @brief Host-side tests for board-level pin maps.
 *
 * Compile:
 *   gcc -Itests/stubs -Icode/system/board -Icode/impl/tc264 \
 *       tests/test_board_map.c code/impl/tc264/tc264_board_map.c \
 *       -o test_board_map.exe
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "tc264_board_map.h"

static void test_pwm_map(void) {
    for (int i = 0; i < SMARTCAR_PWM_ID_MAX; i++) { (void)g_tc264_pwm_map[i]; }
    printf("  PASS test_pwm_map_completeness (%d)\n", SMARTCAR_PWM_ID_MAX);
}
static void test_gpio_map(void) {
    for (int i = 0; i < SMARTCAR_GPIO_ID_MAX; i++) { (void)g_tc264_gpio_map[i]; }
    printf("  PASS test_gpio_map_completeness (%d)\n", SMARTCAR_GPIO_ID_MAX);
}
static void test_encoder_map(void) {
    for (int i = 0; i < SMARTCAR_ENCODER_ID_MAX; i++) {
        (void)g_tc264_encoder_map[i].timer; (void)g_tc264_encoder_map[i].ch1_pin; (void)g_tc264_encoder_map[i].ch2_pin;
    }
    printf("  PASS test_encoder_map_completeness (%d)\n", SMARTCAR_ENCODER_ID_MAX);
}
static void test_pit_map(void) {
    for (int i = 0; i < SMARTCAR_PIT_ID_MAX; i++) { (void)g_tc264_pit_map[i]; }
    printf("  PASS test_pit_map_completeness (%d)\n", SMARTCAR_PIT_ID_MAX);
}
static void test_uart_map(void) {
    for (int i = 0; i < SMARTCAR_UART_ID_MAX; i++) { (void)g_tc264_uart_map[i].uart; (void)g_tc264_uart_map[i].tx; (void)g_tc264_uart_map[i].rx; }
    printf("  PASS test_uart_map_completeness (%d)\n", SMARTCAR_UART_ID_MAX);
}
static void test_pwm_size(void)  { assert(SMARTCAR_PWM_ID_MAX == 5);  printf("  PASS test_pwm_enum_size (%d)\n", SMARTCAR_PWM_ID_MAX); }
static void test_gpio_size(void) { assert(SMARTCAR_GPIO_ID_MAX == 9); printf("  PASS test_gpio_enum_size (%d)\n", SMARTCAR_GPIO_ID_MAX); }

int main(void) {
    printf("=== Board Map Tests ===\n");
    test_pwm_map(); test_gpio_map(); test_encoder_map(); test_pit_map(); test_uart_map();
    test_pwm_size(); test_gpio_size();
    printf("=== ALL PASS ===\n");
    return 0;
}
