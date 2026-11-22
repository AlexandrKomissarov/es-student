// logic/pi-task/pi-task.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

// Состояния задачи расчёта
typedef enum
{
    PI_STATE_IDLE,    // не запускался
    PI_STATE_RUNNING, // считает (порциями)
    PI_STATE_READY   // готово
} pi_state_t;

/**
 * Инициализация задачи (сброс всех переменных).
 */
void pi_task_init(void);

/**
 * Запустить расчёт ряда Лейбница на заданное число членов.
 * 
 * @param terms  Сколько членов ряда нужно посчитать.
 * @return false, если расчёт уже идёт (нельзя запустить второй раз).
 */
bool pi_task_start(uint32_t terms);

/**
 * Обработчик задачи: считает одну порцию членов за один вызов.
 * Вызывать в суперцикле.
 */
void pi_task_handle(void);

/**
 * Получить текущее состояние задачи.
 */
pi_state_t pi_task_get_state(void);

/**
 * Получить результат расчёта.
 * 
 * @param pi       Сюда запишется значение π (если готово).
 * @param time_ms  Сюда запишется время расчёта в миллисекундах (если готово).
 * @return true, если состояние PI_STATE_READY и данные записаны.
 */
bool pi_task_get_result(double *pi, uint32_t *time_ms);

/**
 * Получить количество посчитанных членов ряда (для статуса).
 */
uint32_t pi_task_get_terms_done(void);
