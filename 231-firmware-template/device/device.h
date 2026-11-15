// device.h
#pragma once

#include <stdint.h>

// константы: имя устройства, версия прошивки, проект, ссылка на репозиторий
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.1.0"

#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/AlexandrKomissarov/es-student"

// требование инструкции: фиксированная строка версии SDK
#define PICO_SDK_VERSION_STRING "2.3.0"

// запасное значение DEVICE_BOARD на случай сборки без определения компилятора
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// сведения о плате; печатью занимается api, не device
typedef struct
{
    char name[16];          // имя прошивки
    char version[16];       // версия прошивки, вида 1.0.0
    char project[32];       // имя проекта
    char repo[64];          // адрес репозитория
    char board[16];         // тип платы, значение PICO_BOARD
    char serial[17];        // 16 шестнадцатеричных цифр и нулевой символ
    uint32_t chip_manufacturer; // поле CHIP_ID: производитель, 3 цифры
    uint32_t chip_part;         // поле CHIP_ID: изделие, 4 цифры
    uint32_t chip_revision;     // поле CHIP_ID: ревизия, десятичное число
    char sdk_version[16];   // версия Pico SDK
} device_info_t;

// заполнить паспорт по указателю; при пустом указателе ничего не делает
void device_get_info(device_info_t *info);