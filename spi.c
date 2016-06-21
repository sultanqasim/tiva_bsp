// SPI master and slave modes for Tiva C series microcontrollers
// Copyright (C) 2016 Sultan Qasim Khan

#include <fwlibs.h>
#include <spi.h>

// Format: {SCK, SS, MISO, MOSI, SCK Config, SS Config, MISO Config, MOSI Config}
static const uint32_t SPIPins[4][8] =
{
    {PA2, PA3, PA4, PA5, GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0RX, GPIO_PA5_SSI0TX},
    {PF2, PF3, PF0, PF1, GPIO_PF2_SSI1CLK, GPIO_PF3_SSI1FSS, GPIO_PF0_SSI1RX, GPIO_PF1_SSI1TX},
    {PB4, PB5, PB6, PB7, GPIO_PB4_SSI2CLK, GPIO_PB5_SSI2FSS, GPIO_PB6_SSI2RX, GPIO_PB7_SSI2TX},
    {PD0, PD1, PD2, PD3, GPIO_PD0_SSI3CLK, GPIO_PD1_SSI3FSS, GPIO_PD2_SSI3RX, GPIO_PD3_SSI3TX}
};

static const uint32_t SysCtlSSI[4] =
{
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
};

static const uint32_t SSIBase[4] =
{
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE
};

void spi_init(enum BSP_SPI spi, enum SPI_WIDTH word_size,
        uint32_t data_rate, bool master)
{
    ROM_SysCtlPeripheralEnable(SysCtlSSI[spi]);
    ROM_SysCtlPeripheralSleepEnable(SysCtlSSI[spi]);
    ROM_SysCtlPeripheralEnable(SysCtlGPIOs[SPIPins[spi][0] >> 3]);

    for (int i = 0; i < 4; i++)
    {
        ROM_GPIOPinTypeSSI(GPIO[SPIPins[spi][i] >> 3],
                1 << (SPIPins[spi][i] & 0x7));
        ROM_GPIOPinConfigure(SPIPins[spi][i + 4]);
    }

    // Polarity 0, Phase 0
    ROM_SSIConfigSetExpClk(SSIBase[spi], CLOCK_SPEED, SSI_FRF_MOTO_MODE_0,
            master ? SSI_MODE_MASTER : SSI_MODE_SLAVE, data_rate, word_size);

    SSIEnable(SSIBase[spi]);
}

void spi_write(enum BSP_SPI spi, uint32_t word)
{
    ROM_SSIDataPut(SSIBase[spi], word);
}

uint32_t spi_read(enum BSP_SPI spi)
{
    uint32_t data;
    ROM_SSIDataGet(SSIBase[spi], &data);
    return data;
}
