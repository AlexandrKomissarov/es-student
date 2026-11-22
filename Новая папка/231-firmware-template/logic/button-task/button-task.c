// logic/button-task/button-task.c
#include <stdbool.h>
#include "button-task/button-task.h"
#include "hardware/gpio.h"
#include "systime/systime.h"

// Кнопка подключена между GP15 и землёй (п1.2.2)
#define BUTTON_PIN 15u

// Время дребезга: 20 мс в микросекундах
#define BUTTON_DEBOUNCE_US (20 * 1000)

// Машина состояний кнопки
typedef enum
{
    BTN_RELEASED,
    BTN_PRESSING,
    BTN_PRESSED,
    BTN_RELEASING
} internal_button_state_t;

static internal_button_state_t button_state = BTN_RELEASED;
static uint64_t button_edge_us;
static uint32_t button_presses;

// Колбэк, который вызовет main.c при подтверждённом нажатии
static button_task_callback_t on_press_callback = NULL;

void button_task_init(button_task_callback_t on_press)
{
    // Инициализируем GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Сохраняем колбэк сразу при инициализации
    on_press_callback = on_press;

    // Сброс внутреннего состояния
    button_state = BTN_RELEASED;
    button_edge_us = 0;
    button_presses = 0;
}

void button_task_handle(void)
{
    // Нажатая кнопка замыкает вывод на землю и даёт ноль
    bool pressed = !gpio_get(BUTTON_PIN);
    uint64_t now_us = systime_us();

    switch (button_state)
    {
    case BTN_RELEASED:
        if (pressed)
        {
            button_edge_us = now_us;
            button_state = BUTTON_STATE_PRESSING;
        }
        break;

    case BTN_PRESSING:
        if (!pressed)
        {
            // Уровень не продержался — дребезг или помеха
            button_state = BUTTON_STATE_RELEASED;
        }
        else if (now_us - button_edge_us >= BUTTON_DEBOUNCE_US)
        {
            // Нажатие подтверждено
            button_presses++;
            button_state = BUTTON_STATE_PRESSED;

            // Вызываем колбэк вместо прямого управления LED
            if (on_press_callback != NULL)
            {
                on_press_callback();
            }
        }
        break;

    case BTN_PRESSED:
        if (!pressed)
        {
            button_edge_us = now_us;
            button_state = BUTTON_STATE_RELEASING;
        }
        break;

    case BTN_RELEASING:
        if (pressed)
        {
            // Снова замкнулась — отпускание не подтвердилось
            button_state = BTN_RELEASING;
        }
        else if (now_us - button_edge_us >= BUTTON_DEBOUNCE_US)
        {
            button_state = BTN_RELEASING;
        }
        break;
    }
}

bool button_task_is_pressed(void)
{
    return button_state == BUTTON_STATE_PRESSED ||
           button_state == BUTTON_STATE_RELEASING;
}

uint32_t button_task_get_press_count(void)
{
    return button_presses;
}

bool button_task_get_state(void) {
    return (button_state == BTN_PRESSED);
}