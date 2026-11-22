// logic/api/api.h
#pragma once

#include <stddef.h>
#include "stdio-text-protocol/stdio-text-protocol.h"

// НЕ объявляй здесь command_t — он уже есть в stdio-text-protocol.h

/**
 * Обрабатывает команду и формирует ответ в response_buf.
 * 
 * @param command  Указатель на принятую команду (из stdio-text-protocol)
 * @param response_buf  Буфер, куда api_handle запишет строку ответа
 * @param buf_size  Размер буфера (чтобы не выйти за границы)
 */
void api_handle(const command_t *command, char *response_buf, size_t buf_size);