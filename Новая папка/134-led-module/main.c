#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/unique_id.h"
#include "led.h"
#include "log.h"
#include <stdio.h>
#include "device.h"
#include hardware/regs/sysinfo.h

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
        LOG_INF("led %s\n", led_is_on() ? "on" : "off");
    }
    else if (command == 'd')
    {
        led_set(false);
        LOG_INF("led %s\n", led_is_on() ? "on" : "off");
    }
    else if (command == 'v')
    {
        log_version();
    }
    else if (command == 'i')
    {
        device_info();
    }
    else
    {
        LOG_ERR("unknown command: %c\n", command);
    }
}

int main()
{
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
        volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
        uint32_t id = *chip_id;
        bool current = get_button_debounce(BUTTON_PIN);

        uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
        uint32_t part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
        uint32_t revision = (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;


        if (previous == true && current == false)
        {
            led_toggle();
            LOG_DBG("led %s\n", led_is_on() ? "on" : "off");
        }

        previous = current;
        
        int command = getchar_timeout_us(0);

        if (command == PICO_ERROR_TIMEOUT)
        {
            continue;
        }

        LOG_DBG("got %c\n", command);
        handle_command(command);
    }
}
