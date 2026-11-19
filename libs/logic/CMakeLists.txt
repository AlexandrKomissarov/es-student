# libs/logic/CMakeLists.txt
add_library(logic STATIC
    src/api/api.c              # Теперь путь верный: папка api + файл api.c
    src/api/api-callbacks.c   # Если api-callbacks.c тоже в папке api
    src/led-task/led-task.c    # Если led-task лежит в src/led-task/
    src/button-task/button-task.c
    src/pi-task/pi-task.c
)

target_include_directories(logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include      # <-- свои заголовки
    ${CMAKE_SOURCE_DIR}                     # <-- ЭТО САМОЕ ВАЖНОЕ: теперь он видит корень проекта
)

target_link_libraries(logic
    PUBLIC stdio_text_protocol
    PRIVATE 
        device 
        systime 
        profiling 
        platform 
        logging
        pico_stdlib                          # <-- ЭТА СТРОКА РЕШАЕТ ОШИБКУ С pico/stdio.h
)
