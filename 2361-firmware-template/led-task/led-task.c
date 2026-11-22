// led-task.c
#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "systime/systime.h"

#define LED_TASK_DEFAULT_PERIOD_MS 1000

// переменные задачи — внутреннее дело задачи, static
static led_state_t led_state = LED_STATE_OFF;
static uint32_t led_period_ms = LED_TASK_DEFAULT_PERIOD_MS;
static uint64_t last_us = 0;

// переключить светодиод на противоположный
static void toggle(void)
{
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get_out_level(PICO_DEFAULT_LED_PIN));
}

void led_task_init(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    led_period_ms = LED_TASK_DEFAULT_PERIOD_MS;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    // действие при входе в состояние
    switch (state)
    {
    case LED_STATE_OFF:
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        break;

    case LED_STATE_ON:
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        break;

    case LED_STATE_BLINK:
        // первое переключение сразу, метка времени — с этого момента
        toggle();
        last_us = systime_us();
        break;
    }
}

void led_task_handle(void)
{
    switch (led_state)
    {
    case LED_STATE_OFF:
    case LED_STATE_ON:
        // светодиод уже установлен при входе, делать нечего
        break;

    case LED_STATE_BLINK:
        // период — полный цикл мигания, переключаемся каждые полпериода
        if (systime_period_elapsed(&last_us, (uint64_t)led_period_ms * 1000 / 2))
        {
            toggle();
        }
        break;
    }
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms == 0)
    {
        return false;
    }

    led_period_ms = period_ms;
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return led_period_ms;
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

void led_task_next_state(void)
{
    switch (led_task_get_state())
    {
    case LED_STATE_OFF:
        led_task_set_state(LED_STATE_ON);
        break;

    case LED_STATE_ON:
        led_task_set_state(LED_STATE_BLINK);
        break;

    case LED_STATE_BLINK:
        led_task_set_state(LED_STATE_OFF);
        break;
    }
}