// platform/platform.c
#include "platform/platform.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#define LED_PIN 25        // встроенный светодиод
#define BUTTON_PIN 15     // кнопка 1 — вывод GP15, физический вывод 20
#define BUTTON_PIN_2 14   // кнопка 2 — вывод GP14, физический вывод 19

static const uint8_t button_pins[PLATFORM_BUTTON_COUNT] = {15, 14};

void platform_init(void)
{
    // светодиод
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);   // выключен

    // кнопка 1: вход с подтяжкой к питанию, к земле при нажатии
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // кнопка 2: вход с подтяжкой к питанию, к земле при нажатии
    gpio_init(BUTTON_PIN_2);
    gpio_set_dir(BUTTON_PIN_2, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_2);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on);
}

bool platform_button_read(platform_button_t button)
{
    // с подтяжкой к питанию нажатая кнопка прижимает вывод к земле
    return !gpio_get(button_pins[button]);
}

