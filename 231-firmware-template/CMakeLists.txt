# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(231_firmware_template)

add_compile_options(-Wall)

pico_sdk_init()

# Подключаем libs: внутри них уже есть свои CMakeLists.txt,
# которые создают библиотеки platform, logic, logging, profiling и т.д.
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# Включаем профилирование
target_compile_definitions(profiling PUBLIC PROFILING_ENABLED=1)

# Исполняемый файл: только main.c
add_executable(${PROJECT_NAME}
    main.c
)

# Линкуем проект с библиотеками, которые уже собраны в libs
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    logic
    platform
    profiling
    logging
    device
    systime
    stdio_text_protocol
)


# Добавляем пути к заголовкам.
# Оставляем только текущую папку проекта.
# Все остальные пути уже добавлены через PUBLIC в библиотеках logic и platform.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libs/logic/include
    ${CMAKE_SOURCE_DIR}/libs/logic/include/led-task
    ${CMAKE_SOURCE_DIR}/libs/logic/include/pi-task
    ${CMAKE_SOURCE_DIR}/libs/logic/include/button-task
    ${CMAKE_SOURCE_DIR}/libs/device/include/device
    ${CMAKE_SOURCE_DIR}/libs/logging/include/logging
    ${CMAKE_SOURCE_DIR}/libs/platform/include/platform
    ${CMAKE_SOURCE_DIR}/libs/profiling/include/profiling
    ${CMAKE_SOURCE_DIR}/libs/stdio-text-protocol/include/stdio-text-protocol
    ${CMAKE_SOURCE_DIR}/libs/systime/include/systime


    # добавь сюда остальные подпапки, где лежат твои .h файлы
)

# Настройка вывода
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})
