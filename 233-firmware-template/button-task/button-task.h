//buttn-task.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

// задача кнопки без ожидания: машина состояний подавляет дребезг
void button_task_init(void);
void button_task_handle(void);

// кнопку считают нажатой и в «нажата», и в «отпускание?»:
// пока отпускание не подтвердилось, она ещё нажата
bool button_task_is_pressed(void);

uint32_t button_task_get_press_count(void);

