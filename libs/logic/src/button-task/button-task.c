// button-task/button-task.c
#include "button-task/button-task.h"
#include "platform/platform.h"
#include "systime/systime.h"
#include <stddef.h>

#define DEBOUNCE_TIME_US 50000  // 50 мс

static button_task_callback_t s_on_press = NULL;
static uint32_t press_count = 0;
static bool last_state = false;
static uint64_t debounce_us = 0;

void button_task_init(button_task_callback_t on_press)
{
    s_on_press = on_press;
    press_count = 0;
    last_state = platform_button_read();
    debounce_us = systime_us();
}

// Переименовали handle -> tick
void button_task_tick(void)
{
    bool current = platform_button_read();
    uint64_t now = systime_us();

    if ((now - debounce_us) >= DEBOUNCE_TIME_US)
    {
        if (current != last_state)
        {
            last_state = current;
            debounce_us = now;

            // Кнопка с подтяжкой: нажата, когда LOW (false)
            if (!current)
            {
                press_count++;
                if (s_on_press != NULL)
                {
                    s_on_press();
                }
            }
        }
    }
}

bool button_task_is_pressed(void)
{
    return !platform_button_read();
}

uint32_t button_task_get_press_count(void)
{
    return press_count;
}
