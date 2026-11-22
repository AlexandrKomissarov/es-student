// main.c
#include "platform/platform.h"          // <-- вместо pico/stdlib
#include "firmware.h"
#include "logging/log.h"
#include "device/device.h"
#include "profiling/profiling.h"
#include "stdio-text-protocol/stdio-text-protocol.h"
#include "logic/api/api.h"             // <-- путь по структуре папок
#include "logic/led-task/led-task.h"
#include "logic/button-task/button-task.h"
#include "logic/pi-task/pi-task.h"

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
    platform_init();                    // <-- инициализирует GPIO и stdio

    LOG_INF("%s %s\n", FIRMWARE_NAME, FIRMWARE_VERSION);

    profiling_stopwatch_init(STOPWATCH_LOOP, "loop");
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
    profiling_stopwatch_init(STOPWATCH_LED, "led");
    profiling_stopwatch_init(STOPWATCH_BUTTON, "button");
    profiling_stopwatch_init(STOPWATCH_PI, "pi");

    led_task_init();
    button_task_init();                  // без аргументов, колбэк передаётся через led_task_next_state
    stdio_text_protocol_init();

    while (1)
    {
        profiling_start(STOPWATCH_LOOP);

        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop(STOPWATCH_LED);

        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop(STOPWATCH_BUTTON);

        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop(STOPWATCH_PI);

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
