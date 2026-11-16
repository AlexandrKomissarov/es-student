// systime.c
#include "systime/systime.h"
#include "pico/stdlib.h"

uint64_t systime_us(void)
{
    return time_us_64();
}

uint64_t systime_ms(void)
{
    return time_us_64() / 1000;
}

bool systime_period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    if (last_us == NULL)
    {
        return false;
    }

    uint64_t now_us = time_us_64();

    if (now_us - *last_us < period_us)
    {
        return false;
    }

    // сдвигаем метку на период, а не в текущий момент:
    // так метка остаётся на сетке расписания, и накопленный
    // дрейф гасится при следующем срабатывании
    *last_us += period_us;
    return true;
}