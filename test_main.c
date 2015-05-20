// Test Programs for Tiva BSP
// Copyright (C) 2015 Sultan Qasim Khan

#include <fwlibs.h>
#include <timer.h>
#include <uart.h>

/*
// GPIO and UART test main
static uint8_t udbuff[256];
int main(void)
{
    timer_init();
    uart_init(115200);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x01);
    GPIOPadConfigSet(GPIO_PORTN_BASE, 0x01, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

    // Initial state: LED off
    GPIO_PORTN_DATA_R = 0x00;

    while(1)
    {
        // Toggle the LED
        GPIO_PORTN_DATA_R ^= 0x01;

        // Read the received data and echo it back
        uint32_t bytes_read = uart_read(udbuff, 256, 0);
        uart_write(udbuff, bytes_read, 0);

        // Sleep for a bit
        delay(500);
    }
}
*/
