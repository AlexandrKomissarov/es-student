// api.c
#include "api.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "device.h"
#include "profiling.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

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
        printf("error: zero terms\n");
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
    // печатаем паспорт устройства: имя, версия и остальное по приложению Б
    device_info();
}

static void cmd_uptime(const command_t *command)
{
    // печатаем время с момента запуска прибора в миллисекундах
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}

static void cmd_main_time_exec(const command_t *command)
{
    // печатаем среднее и максимум длительности итерации
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

static void cmd_main_time_reset(const command_t *command)
{
    // сбрасываем максимум длительности итерации
    profiling_reset_max();
    printf("max reset\n");
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
    { "main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec },
    { "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
    { "led_enable", "led on", cmd_led_enable },
    { "led_disable", "led off", cmd_led_disable },
    { "led_blink", "blink the led", cmd_led_blink },
    { "led_period", "set blink period: led_period <period_ms>", cmd_led_period },
    { "button", "button state and presses", cmd_button },
    { "pi_start", "start pi calculation: pi_start [terms]", cmd_pi_start },
    { "pi", "pi result or state", cmd_pi },
};

static const uint api_command_count = sizeof(commands) / sizeof(commands[0]);

static void api_help(void)
{
    // строка о себе и справка всех команд таблицы
    printf("%-17s%s\n", "help", "list of commands");

    for (uint i = 0; i < api_command_count; i++)
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

    for (uint i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, commands[i].name) == 0)
        {
            commands[i].callback(command);
            return;
        }
    }

    printf("error: unknown command '%s', try help\n", command->name);
}

