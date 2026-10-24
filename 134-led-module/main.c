// заголовочные файлы: добавился led.h, константы  больше нет
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "led.h"
#include <stdio.h>

const uint BUTTON_PIN = 15;
const uint DEBOUNCE_MS = 20;

bool get_button_debounce(uint pin)
{
    // читаем вывод устойчиво к дребезгу
    bool state = gpio_get(pin);
    sleep_ms(DEBOUNCE_MS);
    return state && gpio_get(pin);
}

void handle_command(int command)
{
    if (command == 'e')
    {
        led_set(true);
        printf("led %s\n", led_is_on() ? "on" : "off");
    }
    else if (command == 'd')
    {
        led_set(false);
        printf("led %s\n", led_is_on() ? "on" : "off");
    }
    else
    {
        printf("unknown command: %c\n", command);
    }
}

int main()
{
    // включаем стандартный ввод-вывод
    stdio_init_all();

    led_init();

    // инициализируем и настраиваем вывод кнопки
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    // объявляем переменные
    bool led = false;
    bool previous = false;

    while (1)
    {
        // читаем состояние пина кнопки с задержкой
        bool current = get_button_debounce(BUTTON_PIN);

        if (previous == true && current == false)
        {
            led_toggle();
            printf("led %s\n", led_is_on() ? "on" : "off");
        }

        // запоминаем текущее состояние пина кнопки, как предыдущее
        previous = current;
        
        int command = getchar_timeout_us(0);

        if (command == PICO_ERROR_TIMEOUT)
        {
            continue;
        }

        handle_command(command);
    }
}