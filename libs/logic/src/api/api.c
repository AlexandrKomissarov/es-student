// logic/api/api.c — только механизм: поиск команды и вызов
#include "api/api.h"
#include "api/api-commands.h"
#include <stdio.h>
#include <string.h>

// Выводит справку по всем командам из таблицы
static void api_help(void)
{
    printf("%-17s%s\n", "help", "list of commands");
    for (uint32_t i = 0; i < api_command_count; i++)
    {
        printf("%-17s%s\n", api_commands[i].name, api_commands[i].help);
    }
}

void api_handle(const command_t *command)
{
    // Если команда обрезана (слишком длинная) — не выполняем
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    // Команда help обрабатывается прямо здесь
    if (strcmp(command->name, "help") == 0)
    {
        api_help();
        return;
    }

    // Ищем команду в глобальной таблице (она в api-callbacks.c)
    for (uint32_t i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            api_commands[i].callback(command);
            return;
        }
    }

    printf("error: unknown command '%s', try help\n", command->name);
}
