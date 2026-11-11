// clock.c
#include <stdio.h>            // printf
#include "pico/stdlib.h"      // типы SDK
#include "hardware/clocks.h"  // clock_get_hz, имена сигналов

// печать строки таблицы: имя сигнала, настроенная частота, измеренная частота
static void row(const char *name, uint32_t set_khz, uint32_t measured_khz)
{
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

void clk_info(void)
{
    // шапка таблицы: сигнал, настроенная частота, измеренная частота
    printf("%-8s %9s %12s\n", "signal", "set_khz", "measured_khz");

    // настроенная частота — clock_get_hz() в кГц, измеренная — frequency_count_khz()
    row("clk_ref",  clock_get_hz(clk_ref)  / 1000,
                    frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF));
    row("clk_sys",  clock_get_hz(clk_sys)  / 1000,
                    frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS));
    row("clk_peri", clock_get_hz(clk_peri) / 1000,
                    frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI));
    row("clk_usb",  clock_get_hz(clk_usb)  / 1000,
                    frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB));
    row("clk_adc",  clock_get_hz(clk_adc)  / 1000,
                    frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC));

    // ROSC — прочерк вместо настроенной частоты: SDK её не настраивает и не хранит
    printf("%-8s %9s %12u\n", "rosc", "-",
           (unsigned)frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));
}
