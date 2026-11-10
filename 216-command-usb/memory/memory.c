// memory.c
#include <stdio.h>  // printf
#include <stdlib.h>
#include <stdint.h> // uintptr_t
#include "hardware/regs/addressmap.h" // XIP_BASE, SRAM_BASE, ROM_BASE
#include "pico/stdlib.h" // PICO_FLASH_SIZE_BYTES с описанием платы
#include "command.h"
#include "device.h"
#include "led.h"


// размеры ОЗУ и ПЗУ из RP2040 Datasheet, раздел 2.6. Memory
#define SRAM_SIZE (264 * 1024)
#define ROM_SIZE  (16 * 1024)
#define VECTOR_TABLE 0x10000100

int main(void);

uint32_t data_variable = 100;
uint32_t bss_variable;

// символы линкера: внутреннее дело модуля, берём только адреса (&), не значения
extern char __flash_binary_start; // начало образа во флеш
extern char __flash_binary_end;   // конец образа во флеш
extern char __boot2_start__;      // начало загрузчика второй стадии
extern char __boot2_end__;        // конец загрузчика второй стадии
extern char __etext;              // конец кода во flash, здесь хранится копия .data
extern char __data_start__;       // начало .data в ОЗУ
extern char __data_end__;         // конец .data в ОЗУ
extern char __bss_start__;        // начало .bss
extern char __bss_end__;          // конец .bss, здесь же начинается куча
extern char __HeapLimit;          // верхняя граница кучи, конец региона RAM
extern char __StackBottom;        // нижняя граница стека ядра 0
extern char __StackTop;           // вершина стека ядра 0, конец региона SCRATCH_Y

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер
    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");

    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    // sram — базовый адрес из SDK, размер из datasheet
    row("sram", SRAM_BASE, SRAM_BASE + SRAM_SIZE);
    // rom — базовый адрес из SDK, размер из datasheet
    row("rom", ROM_BASE, ROM_BASE + ROM_SIZE);

    // image — от __flash_binary_start до __flash_binary_end
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    // free — от __flash_binary_end до конца флеш-памяти
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    // boot2 — от __boot2_start__ до __boot2_end__
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    // text — от __boot2_end__ до __etext: код и константы
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    // data flash — хранение .data: от __etext, длиной с .data
    row("data flash", (uintptr_t)&__etext,
        (uintptr_t)&__etext + (uintptr_t)(&__data_end__ - &__data_start__));
    // data ram — работа .data: копия в ОЗУ, куда её переносит стартап-код
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    // bss — от __bss_start__ до __bss_end__
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    // heap — от __bss_end__ до __HeapLimit
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    // stack — от __StackBottom до __StackTop
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    // итог: размеры, а не адреса
    unsigned image = (unsigned)(&__flash_binary_end - &__flash_binary_start);
    unsigned boot2 = (unsigned)(&__boot2_end__ - &__boot2_start__);
    unsigned text  = (unsigned)(&__etext - &__boot2_end__);
    unsigned data  = (unsigned)(&__data_end__ - &__data_start__);
    unsigned bss   = (unsigned)(&__bss_end__ - &__bss_start__);
    unsigned heap  = (unsigned)(&__HeapLimit - &__bss_end__);
    unsigned stack = (unsigned)(&__StackTop - &__StackBottom);

    // итог: образ во флеш и из чего он сложился
    printf("total\n");
    printf("  flash image    %8u = boot2 %u + text %u + data %u\n",
           image, boot2, text, data);
    // итог: свободно во флеш-памяти из всего её объёма
    printf("  flash free   %8u of %u\n",
           PICO_FLASH_SIZE_BYTES - image, PICO_FLASH_SIZE_BYTES);
    // итог: занято в ОЗУ — .data и .bss
    printf("  ram used        %8u = data %u + bss %u\n",
           data + bss, data, bss);
    // итог: свободно в ОЗУ — размеры heap и stack из той же таблицы:
    // ОЗУ разбито на три региона, и вычитание из 264 КБ припишет
    // свободной памяти чужие банки
    printf("  ram free      %8u for heap and %u for stack\n", heap, stack);
}
void fw_info(void)
{
    // считаем вызов: обе переменные на единицу больше
    data_variable = data_variable + 1;
    bss_variable = bss_variable + 1;

    // адреса функций со сброшенным признаком Thumb
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)fw_info & ~1u);

    // шапка: объект, адрес, значение
    printf("object          address     value\n");
    printf("%-15s 0x%08x 0x%04x\n", "main",
           (unsigned)(uintptr_t)main, (unsigned)main_code[0]);
    printf("%-15s 0x%08x 0x%04x\n", "fw_info",
           (unsigned)(uintptr_t)fw_info, (unsigned)fw_info_code[0]);

    // адрес массива команд
    printf("%-15s 0x%08x\n", "commands", (unsigned)(uintptr_t)commands);

    // обработчики: имя команды и адрес функции
    for (uint i = 0; i < command_count; i++)
    {
        printf("  - %-12s 0x%08x\n",
               commands[i].name,
               (unsigned)(uintptr_t)commands[i].handler);
    }

    // константы паспорта: имена строк — уже адреса, & не нужен
    printf("%-15s 0x%08x %s\n", "DEVICE_PROJECT",
           (unsigned)(uintptr_t)DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-15s 0x%08x %s\n", "DEVICE_BOARD",
           (unsigned)(uintptr_t)DEVICE_BOARD, DEVICE_BOARD);

    // глобальные переменные
    printf("%-15s 0x%08x %u\n", "data_variable",
           (unsigned)(uintptr_t)&data_variable, data_variable);
    printf("%-15s 0x%08x %u\n", "bss_variable",
           (unsigned)(uintptr_t)&bss_variable, bss_variable);

    // локальная переменная живёт в кадре fw_info() на стеке
    uint32_t stack_variable = 1946;
    // адрес блока в куче — это значение указателя, а не &heap_variable
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    printf("%-15s 0x%08x %u\n", "stack_variable",
           (unsigned)(uintptr_t)&stack_variable, stack_variable);
    printf("%-15s 0x%08x %u\n", "heap_variable",
           (unsigned)(uintptr_t)heap_variable,
           heap_variable != NULL ? *heap_variable : 0);

    // возвращаем блок кучи
    free(heap_variable);
}

void boot_info(void)
{
    // указатель на таблицу векторов и два первых слова из неё
    const uint32_t *vectors = (const uint32_t *)VECTOR_TABLE;
    uint32_t stack_top = vectors[0];
    uint32_t reset_handler = vectors[1];

    // указатель на регистр GPIO_IN и разряд вывода светодиода
    volatile uint32_t *gpio_in = (uint32_t *)0xd0000004;
    uint32_t level = (*gpio_in >> led_pin()) & 1u;

    printf("vector table   0x%08x\n", (unsigned)VECTOR_TABLE);
    printf("  stack top    0x%08x\n", stack_top);
    printf("  reset        0x%08x\n", reset_handler);
    printf("  reset (even) 0x%08x\n", reset_handler & ~1u);
    printf("gpio in        0x%08x\n", (unsigned)(uintptr_t)gpio_in);
    printf("  led bit      %u\n", level);
    printf("  gpio_get     %u\n", gpio_get(led_pin()) ? 1u : 0u);
}
