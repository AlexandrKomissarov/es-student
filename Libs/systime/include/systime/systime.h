// systime.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

// время с момента сброса, микросекунды
uint64_t systime_us(void);

// время с момента сброса, миллисекунды
uint64_t systime_ms(void);

// true — период прошёл с прошлого вызова; обновляет *last_us
bool systime_period_elapsed(uint64_t *last_us, uint64_t period_us);