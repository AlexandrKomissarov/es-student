// logic/led-task/led-task.c
#include "logic/led-task/led-task.h"
#include "pico/stdlib.h"
#include "systime/systime.h"   // Обязательно: без этого systime_us() не виден

// Глобальное состояние (для учебного проекта — нормально)
static led_state_t current_state = LED_STATE_OFF;
static uint32_t blink_period_ms = 500;
static uint64_t next_toggle_time_us = 0;  // uint64_t — чтобы не было переполнения при долгих запусках

void led_task_init(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    led_task_set_state(LED_STATE_OFF);
}

void led_task_set_state(led_state_t state)
{
    current_state = state;

    // Сразу обновляем GPIO для STATIC состояний
    if (current_state == LED_STATE_ON) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    } else {
        // OFF и BLINK в момент установки — гасим LED, мигание начнётся по таймеру
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }

    // Для BLINK сбрасываем таймер, чтобы начать с нужного момента
    if (current_state == LED_STATE_BLINK) {
        next_toggle_time_us = systime_us();
    }
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms < 10 || period_ms > 10000) {
        return false; // слишком быстро или слишком медленно
    }
    blink_period_ms = period_ms;
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return blink_period_ms;
}

led_state_t led_task_get_state(void)
{
    return current_state;
}

void led_task_next_state(void)
{
    switch (current_state) {
        case LED_STATE_OFF:
            current_state = LED_STATE_ON;
            break;
        case LED_STATE_ON:
            current_state = LED_STATE_BLINK;
            break;
        case LED_STATE_BLINK:
            current_state = LED_STATE_OFF;
            break;
    }
    led_task_set_state(current_state);
}

void led_task_handle(void)
{
    // В режимах ON/OFF ничего не делаем: состояние уже установлено, LED горит или погашен
    if (current_state != LED_STATE_BLINK) {
        return;
    }

    uint64_t now_us = systime_us();
    if (now_us >= next_toggle_time_us) {
        // Переключаем состояние
        bool current_value = gpio_get(PICO_DEFAULT_LED_PIN);
        gpio_put(PICO_DEFAULT_LED_PIN, !current_value);

        // Планируем следующее переключение
        next_toggle_time_us += (uint64_t)blink_period_ms * 1000ULL;
    }
}
