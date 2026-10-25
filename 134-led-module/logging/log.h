// log.h

// защита от повторного включения
#pragma once

#include <stdio.h>

// имя устройства, версия прошивки
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

// три уровня и порог LOG_LEVEL
#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_INF 2
#define LOG_LEVEL_DBG 3

void log_version(void);
void log_prefix(const char *level, const char *function, int line);

#define LOG_ERR(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_ERR)             \
        {                                           \
            log_prefix("err", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)
  // уровень "err", проверка порога, подпись и printf
#define LOG_INF(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_INF)             \
        {                                           \
            log_prefix("inf", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)  // уровень "inf"
#define LOG_DBG(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_DBG)             \
        {                                           \
            log_prefix("dbg", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)  // уровень "dbg"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DBG

#define LOG_DBG(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_DBG)             \
        {                                           \
            log_prefix("dbg", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)
#endif    