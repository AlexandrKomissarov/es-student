// main.c
#include "pico/stdlib.h"
#include "firmware.h"
#include "logging/log.h"
#include "device/device.h"
#include "profiling/profiling.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "api.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

// номера секундомеров; имена — из приложения Б
typedef enum
{
    STOPWATCH_LOOP,
    STOPWATCH_COMMAND,
    STOPWATCH_LED,
    STOPWATCH_BUTTON,
    STOPWATCH_PI,
} stopwatch_t;

int main()
{
    // инициализируем стандартный ввод-вывод
    stdio_init_all();

    // журнал сообщает имя и версию прошивки; печатает main.c, не logging
    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    // заводим секундомеры итерации и каждой задачи
    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    // инициализируем задачу светодиода: период 1000 мс, состояние «мигает»
    led_task_init();
    // инициализируем задачу кнопки: вывод GP15 на вход с подтяжкой
    button_task_init();
    // инициализируем протокол строк терминала
    stdio_text_protocol_init();

    while (1)
    {
        profiling_start(STOPWATCH_LOOP);

        // задача светодиода: переключает светодиод по полпериода
        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop(STOPWATCH_LED);

        // задача кнопки: подавляет дребезг машиной состояний
        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop(STOPWATCH_BUTTON);

        // задача расчёта: считает порцию членов ряда за итерацию
        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop(STOPWATCH_PI);

        // принятая строка становится командой, команда — действием прибора
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