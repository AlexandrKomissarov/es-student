// button-task/button-task.h
#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef void (*button_task_callback_t)(void);

void button_task_init(button_task_callback_t on_press);
void button_task_tick(void);          // <-- вместо handle
bool button_task_is_pressed(void);
uint32_t button_task_get_press_count(void);
