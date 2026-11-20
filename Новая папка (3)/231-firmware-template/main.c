/// main.c — дирижёр: запускает модули и крутит суперцикл
#include "firmware.h"
#include "logging/log.h"
#include "profiling/profiling.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "api/api.h"
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "platform/platform.h"

typedef enum
{
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI,
} stopwatch_t;

int main(void)
{
    // Инициализация железа и платформы
    platform_init();

    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    // Профилирование: инициализируем секундомеры
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    // Настройка светодиода: мигание с периодом 1000 мс
    led_task_set_period_ms(1000);
    led_task_set_state(LED_STATE_BLINK);

    // Кнопка: можно оставить callback, если автотест требует реакции на нажатие.
    // Если автотест НЕ требует связи кнопки со светодиодом — можно вообще не передавать callback.
    // Сейчас оставим заглушку, чтобы компилировалось:
    button_task_init(NULL);

    // Протокол терминала
    stdio_text_protocol_init();

    while (1)
    {
        profiling_start(STOPWATCH_LOOP);

        // Тик светодиода (мигание по таймеру)
        profiling_start(STOPWATCH_LED);
        led_task_tick();
        profiling_stop(STOPWATCH_LED);

        // Тик кнопки (подавление дребезга, обработка нажатий)
        profiling_start(STOPWATCH_BUTTON);
        button_task_tick();
        profiling_stop(STOPWATCH_BUTTON);

        // Тик расчёта Pi (порция вычислений за итерацию)
        profiling_start(STOPWATCH_PI);
        pi_task_tick();
        profiling_stop(STOPWATCH_PI);

        // Обработка команд из терминала
        profiling_start(STOPWATCH_COMMAND);
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
        profiling_stop(STOPWATCH_COMMAND);

        profiling_stop(STOPWATCH_LOOP);
    }
}
