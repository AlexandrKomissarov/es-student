// platform.c
#include "platform/platform.h"
#include "pico/stdlib.h"

void platform_init(void)
{
    // platform отвечает только за инициализацию ввода-вывода (UART/USB)
    stdio_init_all();
}