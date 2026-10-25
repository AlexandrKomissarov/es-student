#include "device.h"

#include <stdio.h>
#include "pico/unique_id.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"

void device_info(void)
{
    // читаем серийный номер платы функцией SDK
    printf("project: %s\n", DEVICE_PROJECT);
    // читаем регистр CHIP_ID по адресу и разбираем три поля
    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", board_id);
    // печатаем пять строк паспорта
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", manufacturer, part, revision);
}