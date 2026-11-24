// logic/api/api.h
#pragma once

#include "../../stdio-text-protocol/include/stdio-text-protocol/stdio-text-protocol.h"
#include <stdbool.h>
#include <stdint.h>

// Это то, чего не хватало компилятору!
typedef struct {
    const char *name;      // имя команды (например, "pi_start")
    const char *help;      // описание/помощь
    void (*handler)(const command_t *cmd);  // функция-обработчик
} api_command_t;

void api_handle(const command_t *command);