// device.h
#pragma once

    // константы: имя устройства, версия прошивки, проект, ссылка на репозиторий
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "2112-command-usb"
#define DEVICE_REPO "https://github.com/AlexandrKomissarov/es-student"
#define PICO_SDK_VERSION_STRING "2.3.0"

// запасное значение DEVICE_BOARD на случай сборки без определения компилятора
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

void device_info(void);
