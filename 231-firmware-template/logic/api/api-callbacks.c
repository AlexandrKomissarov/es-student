// logic/api/api-callbacks.c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


#include "api.h"

#include "../../firmware.h"

#include "../../libs/device/include/device/device.h"
#include "../../libs/profiling/include/profiling/profiling.h"
#include "../../libs/systime/include/systime/systime.h"
#include "../../libs/logging/include/logging/log.h"

#include "../led-task/led-task.h"
#include "../button-task/button-task.h"
#include "../pi-task/pi-task.h"
#include "../api/api-commands.h"
#include "../../stdio-text-protocol/include/stdio-text-protocol/stdio-text-protocol.h"

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

    printf("uptime: %llu ms\n", (unsigned long long)(systime_us() / 1000));
}

static void cmd_profiling(const command_t *command)
{
    (void)command;

#if PROFILING_ENABLED
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
#else
    printf("profiling is disabled in this build\n");
#endif
}


static void cmd_profiling_reset(const command_t *command)
{
    (void)command;

    profiling_reset();
    printf("profiling reset\n");
}

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
    // 1. Проверка: должен быть ровно один аргумент
    if (command->argc != 1) {
        printf("error: usage led_period <period_ms>\r\n");
        return;
    }

    const char *arg = command->argv[0];

    // 2. Проверка на пустую строку
    if (!arg || !*arg) {
        printf("error: usage\r\n");
        return;
    }

    // 3. Строгая проверка формата: только цифры 0-9
    // Если первый символ не цифра — сразу ошибка (это покроет "-5", "0x10", "+10" и т.д.)
    if (!isdigit((unsigned char)arg[0])) {
        printf("error: usage\r\n");
        return;
    }

    uint32_t value = 0;
    const char *p = arg;
    bool overflow = false;

    while (*p != '\0') {
        // Если встретили не цифру (например, 'x' в "5x") — ошибка
        if (!isdigit((unsigned char)*p)) {
            printf("error: usage\r\n");
            return;
        }

        // Проверка на переполнение uint32_t (макс 4294967295)
        // value * 10 + digit > 4294967295
        if (value > 429496729 || (value == 429496729 && (*p - '0') > 5)) {
            overflow = true;
            break;
        }

        value = value * 10 + (*p - '0');
        p++;
    }

    if (overflow) {
        printf("error: usage\r\n");
        return;
    }

    // 4. Проверка на ноль: период должен быть больше 0
    if (value == 0) {
        printf("error: period_ms must be greater than 0\r\n");
        return;
    }

    // Всё ок: применяем период
    if (!led_task_set_period_ms(value)) {
        // На случай, если внутри есть ещё какая-то защита
        printf("error: period_ms must be greater than 0\r\n");
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

// таблица команд прибора; help в таблице нет —
// api_handle() печатает справку прямо из этой таблицы
const api_command_t api_commands[] = {
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

// Счётчик команд
const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);
