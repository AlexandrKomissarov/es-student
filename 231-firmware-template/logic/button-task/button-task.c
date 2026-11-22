// logic/button-task/button-task.c
#include "button-task/button-task.h"
#include "platform/platform.h"
#include "systime/systime.h"
#include <stddef.h>

#define BUTTON_DEBOUNCE_US (20 * 1000)

typedef enum
{
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASING,
} button_state_t;

static button_state_t button_state;
static uint64_t button_edge_us;
static uint32_t button_presses;
static void (*g_on_press_cb)(void) = NULL;

// ОДНА функция init: принимает callback и делает всю инициализацию
void button_task_init(void (*on_press)(void))
{
    g_on_press_cb = on_press;          // сохраняем callback
    button_state = BUTTON_STATE_RELEASED;
    button_edge_us = 0;
    button_presses = 0;
}

void button_task_handle(void)
{
    bool pressed = platform_button_read();
    uint64_t now_us = systime_us();

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
            button_state = BUTTON_STATE_RELEASED;
        }
        else if (now_us - button_edge_us >= BUTTON_DEBOUNCE_US)
        {
            button_presses++;
            button_state = BUTTON_STATE_PRESSED;
            
            //  ПРАВИЛЬНО: вызываем callback, который передали из main
            if (g_on_press_cb != NULL)
            {
                g_on_press_cb();
            }
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

