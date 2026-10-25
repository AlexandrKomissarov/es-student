#pragma once

#include <stdio.h>

// имя устройства, версия прошивки
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "134-led-module"
#define DEVICE_REPO "https://github.com/AlexandrKomissarov/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

void device_info(void)

{
    printf("device: %s %s, built %s %s, log level %d\n",
           DEVICE_NAME, FIRMWARE_VERSION, __DATE__, __TIME__, LOG_LEVEL);
}