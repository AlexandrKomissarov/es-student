// logic/pi-task/pi-task.c
#include "pi-task/pi-task.h"
#include <stddef.h>
#include "systime/systime.h"

#define PI_TERMS_PER_CALL 100

// Переменные — только для этого файла, поэтому static
static pi_state_t pi_state = PI_STATE_IDLE;
static double pi_sum;
static double pi_sign;
static uint32_t pi_terms;
static uint32_t pi_k;
static uint64_t pi_start_us;
static uint64_t pi_end_us;

bool pi_task_start(uint32_t terms)
{
    if (pi_state == PI_STATE_RUNNING) {
        return false; // Уже идёт расчёт
    }

    pi_terms = terms;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_start_us = systime_us();
    pi_state = PI_STATE_RUNNING;
    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING) {
        return;
    }

    uint32_t end = pi_k + PI_TERMS_PER_CALL;

    // Проверка на переполнение и ограничение по количеству членов
    if (end < pi_k || end > pi_terms) {
        end = pi_terms;
    }

    for (; pi_k < end; pi_k++) {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k >= pi_terms) {
        pi_end_us = systime_us();
        pi_state = PI_STATE_READY;
    }
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}

// Возвращает результат и время в миллисекундах.
// Форматирование строки "pi: 3.14..." делает api-callbacks.c!
bool pi_task_get_result(double *out_pi, uint32_t *out_time_ms)
{
    if (pi_state != PI_STATE_READY) {
        return false;
    }

    if (out_pi != NULL) {
        *out_pi = pi_sum * 4.0;
    }

    if (out_time_ms != NULL) {
        *out_time_ms = (uint32_t)((pi_end_us - pi_start_us) / 1000);
    }

    return true;
}

// Новая функция: возвращает количество посчитанных членов ряда
uint32_t pi_task_get_terms_done(void)
{
    return pi_k;
}

// Инициализация сбрасывает состояние. Вызывается один раз в main.c
void pi_task_init(void)
{
    pi_state = PI_STATE_IDLE;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_terms = 0;
    pi_k = 0;
    pi_start_us = 0;
    pi_end_us = 0;
}
