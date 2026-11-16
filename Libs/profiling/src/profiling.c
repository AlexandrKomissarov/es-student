// profiling.c
#include <stddef.h>
#include "profiling/profiling.h"
#include "systime/systime.h"

static const char *watch_name[PROFILING_STOPWATCH_COUNT];
static bool watch_live[PROFILING_STOPWATCH_COUNT];

static uint32_t watch_count[PROFILING_STOPWATCH_COUNT];
static uint32_t watch_sum_us[PROFILING_STOPWATCH_COUNT];
static uint32_t watch_max_us[PROFILING_STOPWATCH_COUNT];

#if PROFILING_ENABLED
static uint64_t watch_started_us[PROFILING_STOPWATCH_COUNT];
#endif

void profiling_stopwatch_init(uint32_t id, const char *name)
{
    if (id >= PROFILING_STOPWATCH_COUNT)
    {
        return;
    }

    watch_name[id] = name;
    watch_live[id] = true;
    watch_count[id] = 0;
    watch_sum_us[id] = 0;
    watch_max_us[id] = 0;
}

#if PROFILING_ENABLED
void profiling_start(uint32_t id)
{
    if (id < PROFILING_STOPWATCH_COUNT && watch_live[id])
    {
        watch_started_us[id] = systime_us();
    }
}

void profiling_stop(uint32_t id)
{
    if (id >= PROFILING_STOPWATCH_COUNT || !watch_live[id])
    {
        return;
    }

    uint32_t elapsed_us = (uint32_t)(systime_us() - watch_started_us[id]);

    watch_count[id]++;
    watch_sum_us[id] += elapsed_us;

    if (elapsed_us > watch_max_us[id])
    {
        watch_max_us[id] = elapsed_us;
    }
}
#endif // PROFILING_ENABLED

bool profiling_get(uint32_t id, profiling_result_t *result)
{
    if (id >= PROFILING_STOPWATCH_COUNT || !watch_live[id] || result == NULL)
    {
        return false;
    }

    result->name = watch_name[id];
    result->count = watch_count[id];
    result->mean_us = watch_count[id] > 0
        ? (float)watch_sum_us[id] / watch_count[id]
        : 0.0f;
    result->max_us = watch_max_us[id];
    return true;
}

void profiling_reset(void)
{
    for (uint32_t id = 0; id < PROFILING_STOPWATCH_COUNT; id++)
    {
        watch_count[id] = 0;
        watch_sum_us[id] = 0;
        watch_max_us[id] = 0;
    }
}