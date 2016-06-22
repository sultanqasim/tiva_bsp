// Tiva BSP for TM4C123x
// Copyright (C) 2015-2016 Sultan Qasim Khan

#include <fwlibs.h>
#include <timer.h>
#include <uart.h>
#include <spi.h>

// read one character, block indefinitely
static inline uint8_t uart_read_char(void)
{
    uint8_t c = 0;
    uart_read(&c, 1, -1);
    return c;
}

static inline void uart_write_char(uint8_t c)
{
    uart_write(&c, 1, -1);
}

int main(void)
{
    uint8_t c;
    uint32_t w;
    timer_init();
    uart_init(9600);
    spi_init(TIVA_SPI0, SPI_WORD8, 1000000, true);
    spi_init(TIVA_SPI2, SPI_WORD8, 1000000, false);

    /* perform loopback of UART RX -> SPI0 OUT -> SPI2 IN -> UART TX
     * you must connect:
     * PA2 -> PB4
     * PA3 -> PB5
     * PA5 -> PB6
     */
    while (1)
    {
        c = uart_read_char();
        spi_write(TIVA_SPI0, c);
        w = spi_read(TIVA_SPI2);
        uart_write_char(w & 0xFF);
    }
}
