// led-task.h
#pragma once

#include <stdint.h>
#include <stdbool.h>

// состояния светодиода
typedef enum
{
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK,
} led_state_t;

void led_task_init(void);
void led_task_set_state(led_state_t state);
void led_task_handle(void);
void led_task_next_state(void);

// отказывает в нулевом периоде
bool led_task_set_period_ms(uint32_t period_ms);
uint32_t led_task_get_period_ms(void);

// текущее состояние — командам api нужно отвечать «led: ...»
led_state_t led_task_get_state(void);

