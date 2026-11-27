// platform/platform.c
#include "platform/platform.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include "hardware/adc.h"

#define LED_PIN 25          // встроенный светодиод
#define BUTTON_PIN 15       // кнопка на GP15

// вход АЦП: вывод GP26, канал 0
#define ADC_PIN 26
#define ADC_CHANNEL 0

// опорное напряжение АЦП на плате Pico и число уровней 12-разрядного кода
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

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

    // 3. Напоследок — АЦП: включить модуль и отдать вывод аналоговому входу
    adc_init();
    adc_gpio_init(ADC_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(LED_PIN, on ? 1 : 0);
}

bool platform_button_read(void)
{
    return gpio_get(BUTTON_PIN) == 0; // true, если нажата
}

float platform_voltage_read(void)
{
    adc_select_input(ADC_CHANNEL);
    return adc_read() * ADC_VREF_V / ADC_LEVELS;
}
