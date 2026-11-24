// platform/platform.c
#include "platform/platform.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>

#define LED_PIN 25          // встроенный светодиод
#define BUTTON_PIN 15       // кнопка на GP15

void platform_init(void)
{
    // 1. Сначала — инициализация стандартного ввода/вывода (USB/UART)
    stdio_init_all();

    // 2. Потом — настройка GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);   // выключить светодиод

    gpio_init(BUTTON_PIN);
    // Кнопка подтянута к питанию, нажатие = 0
    gpio_pull_up(BUTTON_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on ? 1 : 0);
}

bool platform_button_read(void)
{
    return gpio_get(BUTTON_PIN) == 0; // true, если нажата
}

