// button-task.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/platform.h"

// функция-колбэк: вызывается без параметров
typedef void (*button_task_callback_t)(void);

// короткое нажатие у кнопки с долгим действием сообщается при отпускании,
// у кнопки без долгого действия — сразу, как подтверждён дребезг.
// долгое нажатие сообщается один раз за удержание дольше секунды
void button_task_init(platform_button_t button, button_task_callback_t on_press, button_task_callback_t on_hold);
void button_task_handle(void);

// кнопку считают нажатой и в «нажата», и в «отпускание»:
// пока отпускание не подтвердилось, она ещё нажата
bool button_task_is_pressed(platform_button_t button);

uint32_t button_task_get_press_count(platform_button_t button);
