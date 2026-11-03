// device.h
#pragma once

    // константы: имя устройства, версия прошивки, проект, ссылка на репозиторий
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "134-led-module"
#define DEVICE_REPO "https://github.com/<AlexandrKomissarov>/es-student"

// запасное значение DEVICE_BOARD на случай сборки без определения компилятора
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

void log_version(void)
{
    printf("device: %s %s, built %s %s, log level %d\n",
           DEVICE_NAME, FIRMWARE_VERSION, __DATE__, __TIME__, LOG_LEVEL);
}

