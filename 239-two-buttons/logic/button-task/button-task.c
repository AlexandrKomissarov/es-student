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

// У каждой кнопки своё состояние, дребезг и счёт нажатий
static button_state_t button_state[PLATFORM_BUTTON_COUNT];
static uint64_t button_edge_us[PLATFORM_BUTTON_COUNT];
static uint32_t button_presses[PLATFORM_BUTTON_COUNT];
static void (*on_press_callbacks[PLATFORM_BUTTON_COUNT])(void) = {NULL};

void button_task_init(platform_button_t button, void (*on_press)(void))
{
    on_press_callbacks[button] = on_press;
    button_state[button] = BUTTON_STATE_RELEASED;
    button_edge_us[button] = 0;
    button_presses[button] = 0;
}

void button_task_handle(void)
{
    for (platform_button_t button = 0; button < PLATFORM_BUTTON_COUNT; button++)
    {
        bool pressed = platform_button_read(button);
        uint64_t now_us = systime_us();

        switch (button_state[button])
        {
        case BUTTON_STATE_RELEASED:
            if (pressed)
            {
                button_edge_us[button] = now_us;
                button_state[button] = BUTTON_STATE_PRESSING;
            }
            break;

        case BUTTON_STATE_PRESSING:
            if (!pressed)
            {
                button_state[button] = BUTTON_STATE_RELEASED;
            }
            else if (now_us - button_edge_us[button] >= BUTTON_DEBOUNCE_US)
            {
                button_presses[button]++;
                button_state[button] = BUTTON_STATE_PRESSED;

                // действие нажатия задаёт тот, кто вызвал init
                if (on_press_callbacks[button] != NULL)
                {
                    on_press_callbacks[button]();
                }
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (!pressed)
            {
                button_edge_us[button] = now_us;
                button_state[button] = BUTTON_STATE_RELEASING;
            }
            break;

        case BUTTON_STATE_RELEASING:
            if (pressed)
            {
                button_state[button] = BUTTON_STATE_PRESSED;
            }
            else if (now_us - button_edge_us[button] >= BUTTON_DEBOUNCE_US)
            {
                button_state[button] = BUTTON_STATE_RELEASED;
            }
            break;
        }
    }
}

bool button_task_is_pressed(platform_button_t button)
{
    return button_state[button] == BUTTON_STATE_PRESSED ||
           button_state[button] == BUTTON_STATE_RELEASING;
}

uint32_t button_task_get_press_count(platform_button_t button)
{
    return button_presses[button];
}