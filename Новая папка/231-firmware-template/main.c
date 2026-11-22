// main.c
#include "pico/stdlib.h"

// Метаданные прошивки
#include "firmware/firmware.h"

// Логирование
#include "logging/log.h"

// Модули платформы и профилирования
#include "platform/platform.h"
#include "profiling/profiling.h"

// Задачи логики (обрати внимание на полные пути!)
#include "logic/led-task/led-task.h"
#include "logic/button-task/button-task.h"
#include "logic/pi-task/pi-task.h"

// Протокол и API
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "api/api.h"

// Номера секундомеров; имена — из приложения Б
typedef enum
{
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI,
} stopwatch_t;

// Колбэк для кнопки: переключает состояние LED
static void on_button_pressed(void)
{
    led_task_next_state();
}

int main(void)
{
    // 1. platform: только stdio (UART/USB) + глобальные настройки
    platform_init();

    // 2. Инициализация задач логики
    // Кнопка сразу получает колбэк — это и есть требование П7
    button_task_init(on_button_pressed);

    led_task_init();
    pi_task_init();

    stdio_text_protocol_init();

    // Журнал сообщает имя и версию прошивки
    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    // Заводим секундомеры итерации и каждой задачи
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    while (true)
    {
        profiling_start(STOPWATCH_LOOP);

        // Задача светодиода
        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop(STOPWATCH_LED);

        // Задача кнопки
        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop(STOPWATCH_BUTTON);

        // Задача расчёта PI
        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop(STOPWATCH_PI);

        // Обработка команды из протокола (только один раз за цикл!)
        profiling_start(STOPWATCH_COMMAND);
        const command_t *cmd = stdio_text_protocol_handle();
        if (cmd != NULL)
        {
            api_handle(cmd);
        }
        profiling_stop(STOPWATCH_COMMAND);

        profiling_stop(STOPWATCH_LOOP);
    }
}

