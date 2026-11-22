// logic/api/api.c
#include "api/api.h"
#include "api/api-commands.h"  // Здесь объявлены api_commands[] и api_command_count
#include <stdio.h>
#include <string.h>

void api_handle(const command_t *command)
{
    if (command == NULL)
    {
        return;
    }

    // Обработка команды help — это часть механизма, а не отдельная команда в таблице
    if (strcmp(command->name, "help") == 0)
    {
        printf("%-17s%s\n", "help", "list of commands");

        for (uint32_t i = 0; i < api_command_count; i++)
        {
            printf("%-17s%s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    // Поиск команды в таблице из api-callbacks.c
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