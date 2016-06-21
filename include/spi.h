// SPI master and slave modes for Tiva C series microcontrollers
// Copyright (C) 2016 Sultan Qasim Khan

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>

enum BSP_SPI
{
    TIVA_SPI0 = 0,
    TIVA_SPI1,
    TIVA_SPI2,
    TIVA_SPI3
};

enum SPI_WIDTH
{
    SPI_WORD4 = 4,
    SPI_WORD8 = 8,
    SPI_WORD16 = 16
};

void spi_init(enum BSP_SPI spi, enum SPI_WIDTH word_size,
        uint32_t data_rate, bool master);
void spi_write(enum BSP_SPI spi, uint32_t word);
uint32_t spi_read(enum BSP_SPI spi);

#endif
