// stdio-text-protocol.c
#include "stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>      // putchar и printf

static char buffer[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t length = 0;
static bool truncated = false;

// команда живёт в буфере модуля и действительна до следующего вызова
static command_t command;

// делим строку на слова на месте: пробелы заменяем нулевыми символами,
// в name и argv записываем указатели на начала слов
static const command_t *make_command(void)
{
    buffer[length] = '\0';
    command.name = buffer;
    command.argc = 0;
    command.truncated = truncated;
    
    uint32_t word = 0;
    bool in_word = false;

    for (uint32_t i = 0; i < length; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '\0';
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true;

            if (word == 0)
            {
                command.name = buffer + i;
            }
            else if (command.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
            {
                command.argv[command.argc] = buffer + i;
                command.argc++;
            }
            else
            {
                // слов больше, чем мест в argv
                command.truncated = true;
            }
        }

    }

    return &command;
}

// сброс состояния: новая строка начнёт заполнять буфер с нуля
void stdio_text_protocol_init(void)
{
    length = 0;
    truncated = false;
}

// читает один символ без ожидания; когда строка закончена,
// возвращает команду, иначе NULL
const command_t *stdio_text_protocol_handle(void)
{
    // читаем символ без ожидания; нет символа — возвращаем NULL
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        // пустую строку пропускаем: не печатаем и не возвращаем
        if (length == 0)
        {
            return NULL;
        }

        // переводим строку в терминале, делим строку на слова
        putchar('\n');

        const command_t *result = make_command();

        // сброс после сборки команды: указатели в ней смотрят
        // в буфер, и он должен остаться целым до следующего вызова
        length = 0;
        truncated = false;

        return result;
    }

    if (symbol == 0x08 || symbol == 0x7f)
    {
        // Backspace: убираем последний символ из буфера
        // и стираем его на экране
        if (length > 0)
        {
            length--;
            printf("\b \b");
        }

        return NULL;
    }

    if (symbol >= 0x20 && symbol <= 0x7e)
    {
        // печатный символ: повторяем в терминал и кладём в буфер
        putchar(symbol);

        if (length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
        {
            buffer[length] = (char)symbol;
            length++;
        }
        else
        {
            // буфер полон: строка принята не целиком
            truncated = true;
        }
    }

    // другие символы (управляющие, стрелки терминала) игнорируем
    return NULL;
}