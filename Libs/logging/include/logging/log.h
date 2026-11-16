// log.h
#pragma once

#include <stdio.h>

#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_INF 2
#define LOG_LEVEL_DBG 3

// умолчательный уровень, если сборка не задала свой
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

void log_prefix(const char *level, const char *function, int line);

// уровень "err": проверка порога, подпись и printf
#define LOG_ERR(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_ERR)             \
        {                                           \
            log_prefix("err", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)

// уровень "inf": проверка порога, подпись и printf
#define LOG_INF(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_INF)             \
        {                                           \
            log_prefix("inf", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)

// уровень "dbg": проверка порога, подпись и printf
#define LOG_DBG(...)                                \
    do                                              \
    {                                               \
        if (LOG_LEVEL >= LOG_LEVEL_DBG)             \
        {                                           \
            log_prefix("dbg", __func__, __LINE__);  \
            printf(__VA_ARGS__);                    \
        }                                           \
    } while (0)

    