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

# профилирование включено по умолчанию в этой сборке;
# PUBLIC — чтобы флаг видела и прошивка через #if !PROFILING_ENABLED
target_compile_definitions(profiling PUBLIC PROFILING_ENABLED=1)
