// logic/api/api-callbacks.c

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>          // <-- нужен для PRIu32

#include "api/api.h"
#include "device/device.h"
#include "profiling/profiling.h"
#include "systime/systime.h"
#include "api/api-commands.h"

// Подключаем паспорт прошивки — единственный источник истины
#include "firmware.h"

// Подключаем модули задач — это убирает ошибки про button_state_t, PI_STATE_IDLE и т.д.
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "led-task/led-task.h"

static void cmd_info(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    device_info_t info;
    device_get_info(&info);

    // Ровно 8 строк в порядке таблицы, как требует ТЗ
    snprintf(response_buf, buf_size,
        "name: %s\r\n"
        "version: %s\r\n"
        "project: %s\r\n"
        "repo: %s\r\n"
        "board: %s\r\n"
        "serial: %s\r\n"
        "chip: 0x%02X-%04X-%02X\r\n"
        "pico-sdk: %s",
        FIRMWARE_NAME,
        FIRMWARE_VERSION,
        FIRMWARE_PROJECT,
        FIRMWARE_REPO,
        info.board,
        info.serial,
        (unsigned)info.chip_manufacturer,
        (unsigned)info.chip_part,
        (unsigned)info.chip_revision,
        info.sdk_version
    );
}

static void cmd_uptime(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    uint64_t ms = systime_us() / 1000;
    snprintf(response_buf, buf_size, "uptime: %llu ms", ms);
}

static void cmd_profiling(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;

    profiling_result_t result;
    size_t written = 0;

    for (uint32_t id = 0; id < PROFILING_STOPWATCH_COUNT; id++) {
        if (profiling_get(id, &result)) {
            char line[128];
            int len = snprintf(line, sizeof(line),
                "%s: count=%u, mean=%u us, max=%u us",
                result.name,
                (unsigned)result.count,
                (unsigned)result.mean_us,
                (unsigned)result.max_us);

            if (len < 0 || len >= (int)sizeof(line)) {
                continue;
            }

            size_t left = buf_size - written;
            if (written == 0) {
                if (left > len) {
                    memcpy(response_buf, line, len);
                    written = len;
                }
            } else {
                if (left >= 2 + len) {
                    response_buf[written++] = '\r';
                    response_buf[written++] = '\n';
                    memcpy(&response_buf[written], line, len);
                    written += len;
                }
            }
        }
    }

    if (written == 0) {
        snprintf(response_buf, buf_size, "profiling is disabled in this build");
    } else {
        response_buf[written] = '\0';
    }
}

static void cmd_profiling_reset(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    profiling_reset();
    snprintf(response_buf, buf_size, "profiling reset");
}

// --- LED команды ---
static void cmd_led_enable(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    led_task_set_state(LED_STATE_ON);
    snprintf(response_buf, buf_size, "led: on, period 1000 ms");
}

static void cmd_led_disable(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    led_task_set_state(LED_STATE_OFF);
    snprintf(response_buf, buf_size, "led: off, period 1000 ms");
}

static void cmd_led_blink(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    led_task_set_state(LED_STATE_BLINK);
    snprintf(response_buf, buf_size, "led: blink, period 500 ms");
}

static void cmd_led_period(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    uint32_t period_ms = 500;

    if (cmd->argc == 1) {
        char *endptr;
        unsigned long val = strtoul(cmd->argv[0], &endptr, 10);

        if (*endptr != '\0' || val == 0 || val > 10000) {
            snprintf(response_buf, buf_size, "error: led_period <period_ms>, min 1 ms, max 10 s");
            return;
        }
        period_ms = (uint32_t)val;
    }

    if (!led_task_set_period_ms(period_ms)) {
        snprintf(response_buf, buf_size, "error: invalid period %lu ms", (unsigned long)period_ms);
        return;
    }

    snprintf(response_buf, buf_size, "led: blink, period %lu ms", (unsigned long)period_ms);
}



// --- Button команда ---
static void cmd_button(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;

    bool is_pressed = button_task_get_state();
    uint32_t press_count = button_task_get_press_count();

    const char *state_str = "released";

    if (is_pressed) {
        state_str = "pressed";
        snprintf(response_buf, buf_size, "Button pressed, count=%lu", (unsigned long)press_count);
    } else {
        snprintf(response_buf, buf_size, "Button idle");
    }

    // Если хочешь именно формат "button: pressed, presses 123", используй этот вариант вместо двух snprintf выше:
    // snprintf(response_buf, buf_size, "button: %s, presses %lu", state_str, (unsigned long)press_count);
}

// --- PI команды ---
static void cmd_pi_start(const command_t *cmd, char *response_buf, size_t buf_size)
{
    uint32_t terms = 1000000;

    if (cmd->argc > 1) {
        snprintf(response_buf, buf_size, "error: usage pi_start [terms], max 1 argument");
        return;
    }

    if (cmd->argc == 1) {
        char *endptr;
        unsigned long val = strtoul(cmd->argv[0], &endptr, 10);
        if (*endptr != '\0' || val == 0 || val > 10000000) {
            snprintf(response_buf, buf_size, "error: pi_start requires positive integer (max 10M)");
            return;
        }
        terms = (uint32_t)val;
    }

    if (!pi_task_start(terms)) {
        snprintf(response_buf, buf_size, "error: pi is busy, stop current calculation first");
        return;
    }

    snprintf(response_buf, buf_size, "pi: started, %lu terms", (unsigned long)terms);
}

static void cmd_pi(const command_t *cmd, char *response_buf, size_t buf_size)
{
    (void)cmd;
    pi_state_t state = pi_task_get_state();

    if (state == PI_STATE_IDLE) {
        snprintf(response_buf, buf_size, "pi: not started");
        return;
    }

    if (state == PI_STATE_RUNNING) {
        snprintf(response_buf, buf_size, "pi: running");
        return;
    }

    double pi_val;
    uint32_t time_ms;
    if (pi_task_get_result(&pi_val, &time_ms)) {
        snprintf(response_buf, buf_size, "pi: %.9f in %lu ms", pi_val, (unsigned long)time_ms);
    } else {
        snprintf(response_buf, buf_size, "pi: error getting result");
    }
}

// Таблица команд
static const api_command_entry commands[] = {
    {"info",        cmd_info,        "device passport"},
    {"uptime",      cmd_uptime,      "time since reset"},
    {"led_enable",  cmd_led_enable,  "turn LED on"},
    {"led_disable", cmd_led_disable, "turn LED off"},
    {"led_blink",   cmd_led_blink,   "blink the LED"},
    {"led_period",  cmd_led_period,  "set blink period"},
    {"button",      cmd_button,      "button state and presses"},
    {"pi_start",    cmd_pi_start,    "start pi calculation"},
    {"pi",          cmd_pi,          "pi result or state"},
    {"profiling",   cmd_profiling,   "stopwatch results"},
    {"profiling_reset", cmd_profiling_reset, "reset stopwatch results"},
};

const api_command_entry* const api_commands = commands;
const uint32_t api_command_count = sizeof(commands) / sizeof(commands[0]);
