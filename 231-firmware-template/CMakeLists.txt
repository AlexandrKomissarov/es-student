# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(231_firmware_template)

add_compile_options(-Wall)

pico_sdk_init()

# Общие библиотеки курса (вне папки проекта)
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

# logic — логика прибора (оставляем как библиотеку)
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

# ВАЖНО: platform.c добавлен прямо в исполняемый файл, без отдельной библиотеки
add_executable(${PROJECT_NAME}
    main.c
    platform/platform.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/logic
)

<<<<<<< HEAD
# Включаем вывод (до линковки библиотек)
=======
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    logging
    device
    systime
    profiling
    stdio_text_protocol
)

# направить стандартный ввод-вывод в USB
>>>>>>> ad4e4178087f61da03d6c13f5f64a96bf2cdbda8
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

<<<<<<< HEAD
# Подключаем все нужные библиотеки
target_link_libraries(${PROJECT_NAME} PRIVATE
    logic
    logging
    stdio_text_protocol
    systime
    profiling
    device
    pico_stdlib          # <-- критично: SDK теперь здесь
)

# Эта команда подключит pico.ld и создаст .uf2
=======
>>>>>>> ad4e4178087f61da03d6c13f5f64a96bf2cdbda8
pico_add_extra_outputs(${PROJECT_NAME})