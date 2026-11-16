// api.c
#include "api.h"
#include "firmware.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include "device/device.h"
#include "profiling/profiling.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"
#include "systime/systime.h"
#include "stdio-text-protocol/stdio-text-protocol.h"

static bool parse_u32(const char *text, uint32_t *number)
{
    if (text == NULL || *text == '\0')
    {
        return false;
    }

    // strtoul() принимает минус; для прибора это не число
    if (*text == '-' || *text == '+')
    {
        return false;
    }

    errno = 0;

    char *end = NULL;
    unsigned long value = strtoul(text, &end, 10);

    if (errno == ERANGE || *end != '\0' || value > UINT32_MAX)
    {
        return false;
    }

    *number = (uint32_t)value;
    return true;
}

static void cmd_pi_start(const command_t *command)
{
    uint32_t terms = 1000000;

    if (command->argc > 1)
    {
        printf("error: usage pi_start [terms]\n");
        return;
    }

    if (command->argc == 1 && !parse_u32(command->argv[0], &terms))
    {
        printf("error: usage pi_start [terms]\n");
        return;
    }

    if (terms == 0)
    {
        printf("error: terms must be greater than 0\n");
        return;
    }

    if (!pi_task_start(terms))
    {
        printf("error: pi is busy\n");
        return;
    }

    printf("pi: started, %u terms\n", (unsigned)terms);
}

static void cmd_pi(const command_t *command)
{
    (void)command;

    switch (pi_task_get_state())
    {
    case PI_STATE_IDLE:
        printf("pi: not started\n");
        break;

    case PI_STATE_RUNNING:
        printf("pi: running\n");
        break;

    case PI_STATE_READY:
    {
        double pi;
        uint32_t time_ms;

        pi_task_get_result(&pi, &time_ms);
        printf("pi: %.9f in %u ms\n", pi, (unsigned)time_ms);
        break;
    }
    }
}

static void cmd_info(const command_t *command)
{
    (void)command;

    // паспорт печатает api: константы прошивки из firmware.h, плата из device
    device_info_t info;
    device_get_info(&info);

    printf("name: %s\n", FIRMWARE_NAME);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    printf("board: %s\n", info.board);
    printf("serial: %s\n", info.serial);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n",
           (unsigned)info.chip_manufacturer,
           (unsigned)info.chip_part,
           (unsigned)info.chip_revision);
    printf("pico-sdk: %s\n", info.sdk_version);
}

static void cmd_uptime(const command_t *command)
{
    (void)command;

    // печатаем время с момента запуска прибора в миллисекундах
    printf("uptime: %llu ms\n", (unsigned long long)(systime_us() / 1000));
}

// печатаем все заведённые секундомеры
static void cmd_profiling(const command_t *command)
{
    (void)command;

#if !PROFILING_ENABLED
    printf("profiling is disabled in this build\n");
    return;
#endif

    profiling_result_t result;

    for (uint32_t id = 0; id < PROFILING_STOPWATCH_COUNT; id++)
    {
        if (profiling_get(id, &result))
        {
            printf("%-10s count %-10u mean %.2f us  max %u us\n",
                   result.name, (unsigned)result.count,
                   result.mean_us, (unsigned)result.max_us);
        }
    }
}

static void cmd_profiling_reset(const command_t *command)
{
    (void)command;

    profiling_reset();
    printf("profiling reset\n");
}

typedef void (*api_callback_t)(const command_t *command);

typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

// единый ответ всех команд светодиода
static void print_led_state(void)
{
    const char *state_name = "off";

    switch (led_task_get_state())
    {
    case LED_STATE_OFF:
        state_name = "off";
        break;

    case LED_STATE_ON:
        state_name = "on";
        break;

    case LED_STATE_BLINK:
        state_name = "blink";
        break;
    }

    printf("led: %s, period %u ms\n", state_name, (unsigned)led_task_get_period_ms());
}

static void cmd_led_enable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_led_blink(const command_t *command)
{
    (void)command;
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

static void cmd_led_period(const command_t *command)
{
    if (command->argc != 1)
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    uint32_t period_ms;

    if (!parse_u32(command->argv[0], &period_ms) ||
        !led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    print_led_state();
}

static void cmd_button(const command_t *command)
{
    (void)command;
    printf("button: %s, presses %u\n",
           button_task_is_pressed() ? "pressed" : "released",
           (unsigned)button_task_get_press_count());
}

// таблица команд со справкой на английском; help в таблице нет —
// api_handle() печатает справку прямо из таблицы
static const api_command_t commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime },
    { "led_enable", "led on", cmd_led_enable },
    { "led_disable", "led off", cmd_led_disable },
    { "led_blink", "blink the led", cmd_led_blink },
    { "led_period", "set blink period: led_period <period_ms>", cmd_led_period },
    { "button", "button state and presses", cmd_button },
    { "pi_start", "start pi calculation: pi_start [terms]", cmd_pi_start },
    { "pi", "pi result or state", cmd_pi },
    { "profiling", "stopwatch results", cmd_profiling },
    { "profiling_reset", "reset stopwatch results", cmd_profiling_reset },
};

static const uint32_t api_command_count = sizeof(commands) / sizeof(commands[0]);

static void api_help(void)
{
    // строка о себе и справка всех команд таблицы
    printf("%-17s%s\n", "help", "list of commands");

    for (uint32_t i = 0; i < api_command_count; i++)
    {
        printf("%-17s%s\n", commands[i].name, commands[i].help);
    }
}

void api_handle(const command_t *command)
{
    // команду с пометкой truncated не выполняем
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    // help выполняется сама, справку берёт из таблицы
    if (strcmp(command->name, "help") == 0)
    {
        api_help();
        return;
    }

    for (uint32_t i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, commands[i].name) == 0)
        {
            commands[i].callback(command);
            return;
        }
    }

    printf("error: unknown command '%s', try help\n", command->name);
}
