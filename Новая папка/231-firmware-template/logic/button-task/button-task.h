// logic/button-task/button-task.h
#pragma once


#include <stdint.h>
#include <stddef.h> // для bool, если компилятор ругается

// Колбэк: вызывается при подтверждённом нажатии кнопки
typedef void (*button_task_callback_t)(void);

// По ТЗ: инициализация сразу с колбэком
void button_task_init(button_task_callback_t on_press);

// Обработка цикла (вызывается в main loop)
void button_task_handle(void);

// Проверка: нажата ли кнопка прямо сейчас
bool button_task_is_pressed(void);

// Счётчик подтверждённых нажатий
uint32_t button_task_get_press_count(void);

// ВАЖНО: добавляем эту функцию, чтобы api-callbacks.c был счастлив.
// Возвращаем true, если состояние == PRESSED (подтверждённое нажатие)
bool button_task_get_state(void); 

