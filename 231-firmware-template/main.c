// main.c
#include "pico/stdlib.h"
#include "profiling.h"
#include "stdio-text-protocol.h"
#include "api.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"

int main()
{
    // инициализируем стандартный ввод-вывод
    stdio_init_all();

    // запоминаем начальное показание часов для профилирования
    profiling_init();
    // инициализируем задачу светодиода: период 1000 мс, состояние «мигает»
    led_task_init();
    // инициализируем задачу кнопки: вывод GP15 на вход с подтяжкой
    button_task_init();
    // инициализируем протокол строк терминала
    stdio_text_protocol_init();

    while (1)
    {
        // учитываем прошлую итерацию
        profiling_iteration();
        // задача светодиода: переключает светодиод по полпериода
        led_task_handle();
        // задача кнопки: подавляет дребезг машиной состояний
        button_task_handle();
        // задача расчёта: считает порцию членов ряда за итерацию
        pi_task_handle();
        

        // принятая строка становится командой, команда — действием прибора
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}
