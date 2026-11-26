// platform/platform.c
#include "platform/platform.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#define LED_PIN 25        // встроенный светодиод
#define BUTTON_1_PIN 15   // кнопка 1 — GP15 (физический вывод 20),
#define BUTTON_2_PIN 14   // кнопка 2 — GP14 (физический вывод 19, соседний с GP15)
// физические выводы платы: GP15 — кнопка 1, GP14 — кнопка 2 (вывод 19, соседний с GP15)
static const uint8_t button_pins[PLATFORM_BUTTON_COUNT] = {BUTTON_1_PIN, BUTTON_2_PIN};

void platform_init(void)
{
    // светодиод
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    // обе кнопки: вход с подтяжкой к питанию, замыкаются на землю при нажатии
    for (platform_button_t button = 0; button < PLATFORM_BUTTON_COUNT; button++)
    {
        gpio_init(button_pins[button]);
        gpio_set_dir(button_pins[button], GPIO_IN);
        gpio_pull_up(button_pins[button]);
    }
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

