# libs/device CMakeLists.txt
add_library(device STATIC
    src/device.c
)

target_include_directories(device PUBLIC
    include
)

# тип платы device получает сам
target_compile_definitions(device PUBLIC
    DEVICE_BOARD="${PICO_BOARD}"
)

target_link_libraries(device PRIVATE
    pico_stdlib
    pico_unique_id
)
