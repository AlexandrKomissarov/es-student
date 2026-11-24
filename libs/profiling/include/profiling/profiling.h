// profiling/include/profiling/profiling.h
#pragma onceg

#include <stdbool.h>
#include <stdint.h>

// число секундомеров задаёт константа
#define PROFILING_STOPWATCH_COUNT 5

// по умолчанию профилирование включено
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

// результат замеров; суммы хранятся в 32 битах —
// при среднем замере в 10 мкс сумма переполнится только через 71 минуту
typedef struct
{
    const char *name;
    uint32_t count;    // число замеров
    float mean_us;     // среднее арифметическое, мкс
    uint32_t max_us;   // максимум, мкс
} profiling_result_t;

// завести секундомер с именем
void profiling_stopwatch_init(uint32_t id, const char *name);

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);

// сбросить все секундомеры
void profiling_reset(void);

#if PROFILING_ENABLED
// пара старт-стоп одного замера
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
// профилирование выключено: макросы пустые, вызовы полностью удаляются компилятором
#define profiling_start(id)
#define profiling_stop(id)
#endif