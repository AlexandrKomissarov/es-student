// pi-task.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

// состояния задачи расчёта
typedef enum
{
    PI_STATE_IDLE,    // не запускался
    PI_STATE_RUNNING, // считает
    PI_STATE_READY,   // готово
} pi_state_t;

// начать расчёт: false, пока идёт расчёт
bool pi_task_start(uint32_t terms);

// считать порцию членов ряда за один вызов
void pi_task_handle(void);

// текущее состояние: «не запускался», «считает», «готово»
pi_state_t pi_task_get_state(void);

// значение и время расчёта; true только в состоянии «готово»
bool pi_task_get_result(double *pi, uint32_t *time_ms);
