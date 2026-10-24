#pragma once

#include "pico/stdlib.h"
#include "led.h"

void led_init(void);
void led_set(bool on);
void led_toggle(void);
bool led_is_on(void);