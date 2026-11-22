// logic/api/api-commands.h
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "stdio-text-protocol/stdio-text-protocol.h"  // command_t объявлен здесь

// Тип для таблицы команд
typedef struct {
    const char *name;                 // имя команды
    void (*handler)(const command_t *cmd, char *buf, size_t size);  // обработчик
    const char *help;                 // описание для справки
} api_command_entry;

extern const api_command_entry* const api_commands;
extern const uint32_t api_command_count;