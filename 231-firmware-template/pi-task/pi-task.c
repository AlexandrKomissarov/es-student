// pi-task.c
#include "pi-task.h"
#include "pico/stdlib.h"

// прикидка: член ряда — около 585 тактов (п2.2.3), при 125 МГц это 4,7 мкс;
// порция должна уложиться в 0,5 мс = 500 мкс, то есть до 106 членов;
// берём 100 членов — около 470 мкс на вызов обработчика
#define PI_TERMS_PER_CALL 100

// переменные расчёта — внутреннее дело задачи, static
static pi_state_t pi_state = PI_STATE_IDLE;
static double pi_sum;       // накопленная сумма ряда
static double pi_sign;      // знак очередного члена
static uint32_t pi_terms;   // сколько членов всего
static uint32_t pi_k;       // сколько членов уже посчитано
static uint64_t pi_start_us;
static uint64_t pi_end_us;

bool pi_task_start(uint32_t terms)
{
    // отказываем, пока идёт расчёт
    if (pi_state == PI_STATE_RUNNING)
    {
        return false;
    }

    pi_terms = terms;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_start_us = time_us_64();
    pi_state = PI_STATE_RUNNING;
    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING)
    {
        return;
    }

    // конец порции: pi_k + PI_TERMS_PER_CALL;
    // возле 4 294 967 295 сумма не помещается в 32 бита и становится
    // маленьким числом — если конец меньше начала, значит, переполнилось
    uint32_t end = pi_k + PI_TERMS_PER_CALL;

    if (end < pi_k || end > pi_terms)
    {
        // переполнение или последний неполный порог: концом порции
        // становится последний член
        end = pi_terms;
    }

    for (; pi_k < end; pi_k++)
    {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k >= pi_terms)
    {
        pi_end_us = time_us_64();
        pi_state = PI_STATE_READY;
    }
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}

bool pi_task_get_result(double *pi, uint32_t *time_ms)
{
    if (pi_state != PI_STATE_READY)
    {
        return false;
    }

    if (pi != NULL)
    {
        *pi = pi_sum * 4.0;
    }

    if (time_ms != NULL)
    {
        *time_ms = (uint32_t)((pi_end_us - pi_start_us) / 1000);
    }

    return true;
}
