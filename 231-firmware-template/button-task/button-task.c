#include "button-task.h"
#include "led-task.h"
#include "pico/stdlib.h"

// кнопка подключена между GP15 и землёй (п1.2.2)
#define BUTTON_PIN 15u

// время дребезга: 20 мс в микросекундах
#define BUTTON_DEBOUNCE_US (20u * 1000u)

// машина состояний кнопки
typedef enum
{
    BUTTON_STATE_RELEASED,  // отпущена
    BUTTON_STATE_PRESSING,  // кандидат на нажатие, ждём 20 мс
    BUTTON_STATE_PRESSED,   // нажата
    BUTTON_STATE_RELEASING, // кандидат на отпускание, ждём 20 мс
} button_state_t;

static button_state_t button_state;
static uint32_t button_edge_us;
static uint32_t button_presses;

void button_task_init(void)
{
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    button_state = BUTTON_STATE_RELEASED;
    button_edge_us = 0;
    button_presses = 0;
}

void button_task_handle(void)
{
    // нажатая кнопка замыкает вывод на землю и даёт ноль
    bool pressed = !gpio_get(BUTTON_PIN);
    uint32_t now_us = time_us_32();

    switch (button_state)
    {
    case BUTTON_STATE_RELEASED:
        if (pressed)
        {
            button_edge_us = now_us;
            button_state = BUTTON_STATE_PRESSING;
        }
        break;

    case BUTTON_STATE_PRESSING:
        if (!pressed)
        {
            // уровень не продержался — дребезг или помеха
            button_state = BUTTON_STATE_RELEASED;
        }
        else if (now_us - button_edge_us >= BUTTON_DEBOUNCE_US)
        {
            // нажатие подтверждено: засчитываем и переключаем светодиод
            button_presses++;
            button_state = BUTTON_STATE_PRESSED;
            led_task_next_state();
        }
        break;

    case BUTTON_STATE_PRESSED:
        if (!pressed)
        {
            button_edge_us = now_us;
            button_state = BUTTON_STATE_RELEASING;
        }
        break;

    case BUTTON_STATE_RELEASING:
        if (pressed)
        {
            // снова замкнулась — отпускание не подтвердилось
            button_state = BUTTON_STATE_PRESSED;
        }
        else if (now_us - button_edge_us >= BUTTON_DEBOUNCE_US)
        {
            button_state = BUTTON_STATE_RELEASED;
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