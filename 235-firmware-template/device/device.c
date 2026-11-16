// device.c
#include <string.h>
#include "device.h"
#include "pico/unique_id.h"
#include "hardware/regs/sysinfo.h"
#include "pico/version.h"

void device_get_info(device_info_t *info)
{
    if (info == NULL)
    {
        return;
    }

    // константы прошивки переписываем в структуру; печать делает api
    strncpy(info->name, DEVICE_NAME, sizeof(info->name) - 1);
    strncpy(info->version, FIRMWARE_VERSION, sizeof(info->version) - 1);
    strncpy(info->project, DEVICE_PROJECT, sizeof(info->project) - 1);
    strncpy(info->repo, DEVICE_REPO, sizeof(info->repo) - 1);
    strncpy(info->board, DEVICE_BOARD, sizeof(info->board) - 1);
    strncpy(info->sdk_version, PICO_SDK_VERSION_STRING, sizeof(info->sdk_version) - 1);

    // серийный номер: массив на 17 символов кладём прямо в структуру
    pico_get_unique_board_id_string(info->serial, sizeof(info->serial));

    // читаем регистр CHIP_ID по адресу и разбираем три поля
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;

    info->chip_manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    info->chip_part = (id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    info->chip_revision = (id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;
}