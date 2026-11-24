# libs/profiling CMakeLists.txt
add_library(profiling STATIC
    src/profiling.c
)

target_include_directories(profiling PUBLIC
    include
)

target_link_libraries(profiling PRIVATE
    pico_stdlib
    systime
)

# По умолчанию профилирование включено, если пользователь не задал PROFILING_ENABLED явно
if(NOT DEFINED PROFILING_ENABLED)
    target_compile_definitions(profiling PRIVATE PROFILING_ENABLED=1)
else()
    # Если пользователь явно задал PROFILING_ENABLED (0 или 1) — используем его значение
    target_compile_definitions(profiling PRIVATE PROFILING_ENABLED=${PROFILING_ENABLED})
endif()
