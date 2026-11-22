// logic/api/api.c
#include "api/api.h"
#include "api/api-commands.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Вспомогательная функция для поиска команды (регистронезависимо)
static const api_command_entry* find_command(const char *name)
{
    for (uint32_t i = 0; i < api_command_count; ++i) {
        if (strcasecmp(name, api_commands[i].name) == 0) {
            return &api_commands[i];
        }
    }
    return NULL;
}

void api_handle(const command_t *command, char *response_buf, size_t buf_size)
{
    // Очищаем буфер ответа
    response_buf[0] = '\0';

    if (!command || command->name == NULL) {
        snprintf(response_buf, buf_size, "error: empty command");
        return;
    }

    // Команду с пометкой truncated не выполняем
    if (command->truncated) {
        snprintf(response_buf, buf_size, "error: truncated command");
        return;
    }

     // Обработка команды help
    if (strcmp(command->name, "help") == 0) {
        size_t written = 0;
        for (uint32_t i = 0; i < api_command_count; ++i) {
            size_t name_len = strlen(api_commands[i].name);
            size_t help_len = strlen(api_commands[i].help);
            // name + пробел + help + \r\n + 1 байт на '\0'
            size_t needed = name_len + 1 + help_len + 2 + 1;

            if (written + needed > buf_size) {
                break; // не влезает — останавливаемся
            }

            int ret = snprintf(response_buf + written, buf_size - written, "%-16s %s\r\n",
                               api_commands[i].name,
                               api_commands[i].help);

            // snprintf возвращает количество записанных символов без '\0', но мы уже учли запас
            if (ret < 0 || (size_t)ret >= buf_size - written) {
                break;
            }
            written += ret;
        }
        return;
    }

    const api_command_entry *entry = find_command(command->name);

    if (!entry) {
        snprintf(response_buf, buf_size, "error: unknown command '%s'. Type 'help' for list.", command->name);
        return;
    }

    // Вызываем обработчик из api-callbacks.c — он сам напишет в response_buf
    entry->handler(command, response_buf, buf_size);
}
