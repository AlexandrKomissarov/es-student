# libs/platform/CMakeLists.txt
add_library(platform STATIC
    src/platform.c
)

# ГЛАВНОЕ: говорим, что заголовки лежат в папке include внутри ЭТОГО модуля
target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(platform PRIVATE pico_stdlib)
