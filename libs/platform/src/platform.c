// platform.c
#include "platform/platform.h"
#include "pico/stdlib.h" // Только здесь в проекте!

// Номера пинов — только здесь
#define LED_PIN 25
#define BTN_PIN 15

void platform_init(void) 
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN); // Кнопка замыкает на землю -> 0
}

void platform_led_set(bool on) {
    gpio_put(LED_PIN, on);
}

bool platform_button_read(void) {
    // Если кнопка нажата — на пине 0. Нам нужно true, если нажата.
    return !gpio_get(BTN_PIN);
}