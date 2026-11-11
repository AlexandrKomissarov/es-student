// clock.c
#include <stdio.h>            // printf
#include "pico/stdlib.h"      // типы SDK
#include "hardware/clocks.h"  // clock_get_hz, имена сигналов
#include "log.h"              // LOG_INF, LOG_ERR

// пониженная частота ядра: вдвое ниже рабочей 125 МГц
const uint32_t CLK_SYS_LOW_KHZ = 62500;

static void clk_sys_set(uint32_t khz)
{
    if (set_sys_clock_khz(khz, false))
    {
        LOG_INF("clk_sys %u kHz\n", (unsigned)khz);
    }
    else
    {
        LOG_ERR("clk_sys %u kHz is not set\n", (unsigned)khz);
    }
}

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

// перевод ядра на пониженную частоту
void clk_sys_low(void)
{
    clk_sys_set(CLK_SYS_LOW_KHZ);
}

// перевод ядра на рабочую частоту из Pico SDK
void clk_sys_default(void)
{
    clk_sys_set(SYS_CLK_KHZ);
}

void uptime(void)
{
    // показание монотонного времени таймера с момента сброса, в миллисекундах
    printf("uptime: %llu ms\n", time_us_64() / 1000);
}
