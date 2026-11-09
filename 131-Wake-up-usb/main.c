#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    while (1)
    {
        printf("Wake up, Neo. You obosralsya.\n");
        sleep_ms(5000);
    }
}
