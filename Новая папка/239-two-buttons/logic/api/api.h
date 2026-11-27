// logic/api/api.h
#pragma once

#include "../../stdio-text-protocol/include/stdio-text-protocol/stdio-text-protocol.h"
#include <stdbool.h>

// Только интерфейс: что можно вызвать снаружи
void api_handle(const command_t *command);