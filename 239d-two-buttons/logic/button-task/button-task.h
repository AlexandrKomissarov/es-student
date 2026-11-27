// button-task.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/platform.h"

// задача кнопки без ожидания: машина состояний подавляет дребезг.
// кнопки адресуются номером: сколько их на плате, знает платформа.
// короткое нажатие у кнопки с долгим действием сообщается при отпускании,
// у кнопки без долгого действия — сразу, как подтверждён дребезг
void button_task_init(platform_button_t button, void (*on_press)(void), void (*on_hold)(void));
void button_task_handle(void);

// кнопку считают нажатой и в «нажата», и в «отпускание»:
// пока отпускание не подтвердилось, она ещё нажата
bool button_task_is_pressed(platform_button_t button);

uint32_t button_task_get_press_count(platform_button_t button);