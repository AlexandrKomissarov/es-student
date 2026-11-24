// logic/api/api-commands.h
#pragma once
#include <stddef.h>   // <-- вот эта строчка всё исправит
#include "api.h"  // <-- теперь тут лежит api_command_t

extern const api_command_t api_commands[];
extern const size_t api_command_count;