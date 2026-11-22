// logic/led-task/led-task.c
#include "../include/led-task/led-task.h"
#include "../include/platform/platform.h"
#include "../include/systime/systime.h"  // для systime_us()

#include <stdbool.h>
#include <stdint.h>

static led_state_t current_state = LED_STATE_OFF;
static uint32_t blink_period_ms = 500;
static uint64_t last_toggle_us = 0;
static bool blink_is_on = false;

void led_task_set_state(led_state_t state)
{
    current_state = state;

    if (state == LED_STATE_ON) {
        blink_is_on = true;
        platform_led_set(true);
    } else if (state == LED_STATE_OFF) {
        blink_is_on = false;
        platform_led_set(false);
    } else {
        // LED_STATE_BLINK: сразу включаем, дальше будет переключать tick
        blink_is_on = true;
        platform_led_set(true);
        last_toggle_us = systime_us();  // сбрасываем таймер, чтобы не было скачка
    }
}

led_state_t led_task_get_state(void)
{
    return current_state;
}

bool led_task_set_period_ms(uint32_t ms)
{
    if (ms == 0) return false;
    blink_period_ms = ms;
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return blink_period_ms;
}

void led_task_tick(void)
{
    // Мигать нужно только в режиме BLINK
    if (current_state != LED_STATE_BLINK) {
        return;
    }

    uint64_t now_us = systime_us();
    uint64_t period_us = (uint64_t)blink_period_ms * 1000ULL;

    if ((now_us - last_toggle_us) >= period_us) {
        last_toggle_us = now_us;
        blink_is_on = !blink_is_on;
        platform_led_set(blink_is_on);
    }
}
