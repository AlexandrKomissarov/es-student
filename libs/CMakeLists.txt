# libs/CMakeLists.txt

# Сначала подключаем независимые библиотеки
add_subdirectory(stdio-text-protocol)
add_subdirectory(systime)
add_subdirectory(profiling)
add_subdirectory(device)
add_subdirectory(logging)

# Потом platform (ему нужен pico_stdlib, он базовый)
add_subdirectory(platform)

# В конце logic (он зависит от platform, logging, device и т.д.)
add_subdirectory(logic)