// platform.h
#pragma once

/**
 * platform_init: только инициализация UART/USB (stdio).
 * GPIO для LED и кнопки инициализируются внутри своих задач (logic).
 */
void platform_init(void);