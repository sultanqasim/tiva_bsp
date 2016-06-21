// Tiva BSP for TM4C123x
// Copyright (C) 2015-2016 Sultan Qasim Khan

#include <fwlibs.h>
#include <timer.h>
#include <uart.h>
#include <commands.h>

// read one character, block indefinitely
static inline uint8_t uart_read_char(void)
{
    uint8_t c = 0;
    uart_read(&c, 1, -1);
    return c;
}


static inline uint32_t uart_read_word()
{
    // data should arrive in little endian form
    uint8_t buff[4];
    uint32_t word;
    uart_read(buff, 4, -1);
    word = buff[0] | (buff[1]<<8) | (buff[2]<<16) | (buff[3]<<24);
    return word;
}


static inline void uart_write_word(uint32_t w)
{
    // be a good boy and don't violate strict aliasing rules
    uint8_t buff[4] = {w & 0xFF, (w>>8) & 0xFF, (w>>16) & 0xFF, (w>>24) & 0xFF};
    uart_write(buff, 4, -1);
}


static void detect_syncword(void);
static void send_syncword_cmd(uint8_t cmd);
static void handle_example(void);


int main(void)
{
    timer_init();
    uart_init(800000);

    // use GPIO port F LEDs for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x0E);
    GPIOPadConfigSet(GPIO_PORTF_BASE, 0x0E, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIO_PORTF_DATA_R = 0;

    uint8_t c;

    // handle commands in an endless loop
    while (1)
    {
        detect_syncword();

        // read in the command byte
        c = uart_read_char();

        switch (c)
        {
        case CMD_EXAMPLE:
            handle_example();
            break;
        case CMD_SETGPIO:
            GPIO_PORTF_DATA_R = uart_read_char();
            send_syncword_cmd(CMD_SETGPIO);
            break;
        default:
            // send a NAK
            send_syncword_cmd(CMD_NAK);
            break;
        }
    }
}


// wait for 0xDEADBEEF (little endian) sync word over UART
static void detect_syncword(void)
{
    while (1)
    {
        if (uart_read_char() != 0xEF) continue;
        if (uart_read_char() != 0xBE) continue;
        if (uart_read_char() != 0xAD) continue;
        if (uart_read_char() == 0xDE) break;
    }
}


static void send_syncword_cmd(uint8_t cmd)
{
    uint8_t sw_bytes[5] = {0xCE, 0xFA, 0x0D, 0xF0, 0x00};
    sw_bytes[4] = cmd;
    uart_write(sw_bytes, 5, -1);
}


static void handle_example(void)
{
    send_syncword_cmd(CMD_EXAMPLE);
}
