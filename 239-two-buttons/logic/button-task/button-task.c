// logic/button-task/button-task.c
#include "button-task/button-task.h"
#include "platform/platform.h"
#include "systime/systime.h"
#include <stddef.h>

#define BUTTON_DEBOUNCE_US (20 * 1000)
#define BUTTON_HOLD_US 1000000

typedef enum
{
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSING,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASING,
} button_state_t;

// у каждой кнопки своё состояние, дребезг и счёт нажатий
static button_state_t button_state[PLATFORM_BUTTON_COUNT];
static uint64_t button_edge_us[PLATFORM_BUTTON_COUNT];
static uint64_t button_hold_start_us[PLATFORM_BUTTON_COUNT];
static bool button_hold_reported[PLATFORM_BUTTON_COUNT];
static uint32_t button_presses[PLATFORM_BUTTON_COUNT];

static button_task_callback_t on_press_callbacks[PLATFORM_BUTTON_COUNT] = {NULL};
static button_task_callback_t on_hold_callbacks[PLATFORM_BUTTON_COUNT] = {NULL};

void button_task_init(platform_button_t button, button_task_callback_t on_press, button_task_callback_t on_hold)
{
    on_press_callbacks[button] = on_press;
    on_hold_callbacks[button] = on_hold;
    button_state[button] = BUTTON_STATE_RELEASED;
    button_edge_us[button] = 0;
    button_hold_start_us[button] = 0;
    button_hold_reported[button] = false;
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
                button_hold_start_us[button] = now_us;
                button_hold_reported[button] = false;
                button_state[button] = BUTTON_STATE_PRESSED;

                // у кнопки без долгого действия нажатие — сразу, как раньше
                if (on_hold_callbacks[button] == NULL && on_press_callbacks[button] != NULL)
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
            else if (!button_hold_reported[button] &&
                     now_us - button_hold_start_us[button] >= BUTTON_HOLD_US)
            {
                // долгое нажатие сообщается один раз за нажатие
                button_hold_reported[button] = true;
                if (on_hold_callbacks[button] != NULL)
                {
                    on_hold_callbacks[button]();
                }
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

                // у кнопки с долгим действием короткое сообщаем при отпускании,
                // но только если это нажатие не стало долгим
                if (on_hold_callbacks[button] != NULL && !button_hold_reported[button] &&
                    on_press_callbacks[button] != NULL)
                {
                    on_press_callbacks[button]();
                }
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