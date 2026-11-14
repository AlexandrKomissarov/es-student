// profiling.c
#include "profiling.h"
#include "pico/stdlib.h"

// итерация около 2 мкс, за секунду их около 500 000;
// ближайшая степень двойки 2^19 = 524 288, среднее помнит около секунды
#define AVG_SHIFT 19

// замер в 1/256 долях микросекунды, чтобы среднее не теряло дробную часть
#define FRACTION_SHIFT 8

// показание часов на прошлой итерации
static uint32_t previous_us = 0;

// самый долгий замер итерации
static uint32_t max_us = 0;

// сумма для экспоненциального среднего, в 1/256 долях микросекунды;
// в N раз больше среднего, N = 2^AVG_SHIFT
static uint64_t avg_sum = 0;

void profiling_init(void)
{
    // запоминаем начальное показание часов
    previous_us = time_us_32();
}

void profiling_iteration(void)
{
    // разность показаний равна длительности прошлой итерации целиком
    uint32_t now_us = time_us_32();
    uint32_t iteration_us = now_us - previous_us;
    previous_us = now_us;

    if (iteration_us > max_us)
    {
        max_us = iteration_us;
    }

    // вычитаем N-ю часть суммы и прибавляем новый замер
    avg_sum = avg_sum - (avg_sum >> AVG_SHIFT) + ((uint64_t)iteration_us << FRACTION_SHIFT);
}

float profiling_avg_us(void)
{
    // переводим из 1/256 долей микросекунды в микросекунды
    return (float)(avg_sum >> AVG_SHIFT) / (1 << FRACTION_SHIFT);
}

uint32_t profiling_max_us(void)
{
    return max_us;
}

void profiling_reset_max(void)
{
    max_us = 0;
}