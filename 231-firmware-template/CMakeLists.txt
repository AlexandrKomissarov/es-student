# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(231_firmware_template)

add_compile_options(-Wall)

pico_sdk_init()

# Общие библиотеки курса
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# выключатель профилирования: 0 — вызовов секундомеров в образе нет
target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1
)
# 1. Библиотека platform (отдельная!)
add_library(platform STATIC
    platform/platform.c
)
target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# ДОБАВЬ ЭТУ СТРОКУ (она разрешает #include <pico/stdlib.h>)
target_link_libraries(platform PRIVATE pico_stdlib)

# 2. Библиотека logic (БЕЗ platform.c!)
add_library(logic STATIC
    logic/api/api.c
    logic/api/api-callbacks.c
    logic/led-task/led-task.c
    logic/button-task/button-task.c
    logic/pi-task/pi-task.c
)

target_include_directories(logic PUBLIC
    ${CMAKE_SOURCE_DIR}/logic
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(logic PUBLIC stdio_text_protocol)

target_link_libraries(logic PRIVATE
    systime
    profiling
    logging
    device
)

# 3. ЕДИНСТВЕННАЯ цель прошивки
# Если переменная FIRMWARE_NAME не определена, зададим её явно
if(NOT DEFINED FIRMWARE_NAME)
    set(FIRMWARE_NAME 231_firmware_template)
endif()

add_executable(${FIRMWARE_NAME}
    main.c
)

target_link_libraries(${FIRMWARE_NAME} PRIVATE
    logic
    platform
    stdio_text_protocol
    systime
    profiling
    device
    logging
    pico_stdlib
)

target_include_directories(${FIRMWARE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/logic
)

# Вывод и .uf2
pico_enable_stdio_usb(${FIRMWARE_NAME} 1)
pico_enable_stdio_uart(${FIRMWARE_NAME} 0)
pico_add_extra_outputs(${FIRMWARE_NAME})
