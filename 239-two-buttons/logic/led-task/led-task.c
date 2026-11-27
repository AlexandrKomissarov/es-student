// logic/led-task/led-task.c
#include "led-task/led-task.h"
#include "platform/platform.h"
#include "systime/systime.h"

#define LED_TASK_DEFAULT_PERIOD_MS 1000

// Статические переменные состояния (хранятся внутри модуля)
static led_state_t led_state = LED_STATE_OFF;
static uint32_t led_period_ms = LED_TASK_DEFAULT_PERIOD_MS;
static uint64_t last_us = 0;
static bool led_current_value = false;

static const uint32_t led_task_periods[] = {250, 500, 1000};
#define LED_TASK_PERIODS_COUNT (sizeof(led_task_periods) / sizeof(led_task_periods[0]))

static void set_led(bool on)
{
    led_current_value = on;
    platform_led_set(on);
}

static void toggle(void)
{
    set_led(!led_current_value);
}

void led_task_init(void)
{
    // platform уже инициализирован в main.c
    led_period_ms = LED_TASK_DEFAULT_PERIOD_MS;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    switch (state)
    {
    case LED_STATE_OFF:
        set_led(false);
        break;

    case LED_STATE_ON:
        set_led(true);
        break;

    case LED_STATE_BLINK:
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
        return false;

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

void led_task_next_period(void)
{
    // периодом владеет задача светодиода, поэтому меняем его через set
    for (uint32_t i = 0; i < LED_TASK_PERIODS_COUNT; i++)
    {
        if (led_task_periods[i] == led_period_ms)
        {
            led_task_set_period_ms(led_task_periods[(i + 1) % LED_TASK_PERIODS_COUNT]);
            return;
        }
    }

    // текущий период вне круга (например, 2000 из команды led_period):
    // начинаем круг с начала
    led_task_set_period_ms(led_task_periods[0]);
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

void led_task_blink(void)
{
    led_task_set_state(LED_STATE_BLINK);
}
