// Software-buffered UART functions for Tiva C series microcontrollers
// Copyright (C) 2015 Sultan Qasim Khan

#include <fwlibs.h>
#include <uart.h>

// Naming convention: write to head, read from tail

// LED setting macros
#define SET_RXLED(x) HWREG(GPIO_PORTN_BASE + (GPIO_PIN_0 << 2)) = ((x) ? GPIO_PIN_0 : 0)
#define SET_TXLED(x) HWREG(GPIO_PORTN_BASE + (GPIO_PIN_1 << 2)) = ((x) ? GPIO_PIN_1 : 0)

#define UART_BASE UART0_BASE

// UART read buffer
#define READ_BUFF_SIZE 4096
static uint8_t uart_read_buff[READ_BUFF_SIZE];
static volatile int32_t urb_head = 0;
static volatile int32_t urb_tail = 0;
static volatile bool rb_overflow = false;

// UART write buffer
#define WRITE_BUFF_SIZE 8192
static uint8_t uart_write_buff[WRITE_BUFF_SIZE];
static volatile int32_t uwb_head;
static volatile int32_t uwb_tail;

static inline int32_t inc_roll(int32_t val, int32_t roll)
{
    int32_t ret = val + 1;
    if (ret >= roll) ret -= roll;
    return ret;
}

static inline int32_t rx_qsize()
{
    int32_t ret = urb_head - urb_tail;
    if (ret < 0) ret += WRITE_BUFF_SIZE;
    return ret;
}


// Assumes buffer has room
static inline void rx_enqueue(uint8_t c)
{
    uart_read_buff[urb_head] = c;
    urb_head = inc_roll(urb_head, READ_BUFF_SIZE);
}


// Assumes buffer is not empty
static inline uint8_t rx_dequeue()
{
    uint8_t c = uart_read_buff[urb_tail];
    urb_tail = inc_roll(urb_tail, READ_BUFF_SIZE);
    return c;
}


static inline int32_t tx_qsize()
{
    int32_t ret = uwb_head - uwb_tail;
    if (ret < 0) ret += WRITE_BUFF_SIZE;
    return ret;
}


// Assumes buffer has room
static inline void tx_enqueue(uint8_t c)
{
    uart_write_buff[uwb_head] = c;
    uwb_head = inc_roll(uwb_head, WRITE_BUFF_SIZE);
}


// Assumes buffer is not empty
static inline uint8_t tx_dequeue()
{
    uint8_t c = uart_write_buff[uwb_tail];
    uwb_tail = inc_roll(uwb_tail, WRITE_BUFF_SIZE);
    return c;
}


// Called when UART RX FIFO has data in it
// This is called from ISR for the UART RX interrupt
static void flush_read_fifo()
{
    // This is not reentrant, so disable its interrupt while running
    ROM_UARTIntDisable(UART_BASE, UART_INT_RX);

    // Clear the interrupt flag
    ROM_UARTIntClear(UART_BASE, UART_INT_RX);

    // Read from the FIFO and place in the circular buffer in memory
    while (ROM_UARTCharsAvail(UART_BASE))
    {
        if (rx_qsize() < READ_BUFF_SIZE - 1)
        {
            rx_enqueue(ROM_UARTCharGet(UART_BASE));
        }
        else
        {
            // Any new data will be discarded till room is available in queue
            rb_overflow = true;
            break;
        }
    }

    if (rx_qsize()) SET_RXLED(1);

    // Re-enable the interrupt
    ROM_UARTIntEnable(UART_BASE, UART_INT_RX);
}


// Called when UART TX FIFO needs to be fed with data
// This is called from ISR for UART TX interrupt
static void flush_write_fifo()
{
    // This is not reentrant, so disable its interrupt while running
    ROM_UARTIntDisable(UART_BASE, UART_INT_TX);

    // Clear the interrupt flag
    ROM_UARTIntClear(UART_BASE, UART_INT_TX);

    // Fill up the FIFO
    while (ROM_UARTSpaceAvail(UART_BASE) && tx_qsize())
    {
        ROM_UARTCharPut(UART_BASE, tx_dequeue());
    }

    // Re-enable the interrupt if the queue is not empty
    if (tx_qsize())
        ROM_UARTIntEnable(UART_BASE, UART_INT_TX);
    else
        SET_TXLED(0);
}


void uart_isr()
{
    int32_t status = ROM_UARTIntStatus(UART_BASE, false);
    if (status & UART_INT_RX) flush_read_fifo();
    if (status & UART_INT_TX) flush_write_fifo();
}


static int32_t uart_time()
{
    return (0xFFFFFFFF - ROM_TimerValueGet(TIMER3_BASE, TIMER_A)) / MS_CYCLES;
}


static void reset_uart_timer()
{
    ROM_TimerLoadSet(TIMER3_BASE, TIMER_A, 0xFFFFFFFF);
}


void uart_init(int32_t baud_rate)
{
    // UART0 pins are PA0 and PA1

    // Enable the UART peripheral in SysCtl
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);

    // Configure the associated GPIO pins for UART
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, 0x3);

    // Configure the UART
    ROM_UARTConfigSetExpClk(UART_BASE, CLOCK_SPEED, baud_rate,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    ROM_UARTFIFOEnable(UART_BASE);

    // Configure the UART interrupts
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART_BASE, UART_INT_RX | UART_INT_TX);
    ROM_UARTFIFOLevelSet(UART_BASE, UART_FIFO_TX2_8, UART_FIFO_RX6_8);
    UARTIntRegister(UART_BASE, uart_isr);

    // Enable the UART
    ROM_UARTEnable(UART_BASE);

    // Configure TIMER3 for UART timeouts
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER3);
    ROM_TimerConfigure(TIMER3_BASE, TIMER_CFG_ONE_SHOT);
    ROM_TimerEnable(TIMER3_BASE, TIMER_A);

    // Enable PN0 and PN1 and RX and TX LEDs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x3);
    GPIOPadConfigSet(GPIO_PORTN_BASE, 0x3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIO_PORTN_DATA_R = 0x0;
}


// timeout in milliseconds
// timeout 0 means timeout right away
// negative timeout means wait forever
uint32_t uart_read(void *vdata, uint32_t size, int32_t timeout)
{
    uint8_t *data = (uint8_t *)vdata;
    uint32_t bytes_read = 0;

    // Disable UART receive interrupt to avoid race condition
    ROM_UARTIntDisable(UART_BASE, UART_INT_RX);

    flush_read_fifo();

    // read available data till timeout
    if (timeout < 0)
    {
        // block forever, keep flushing FIFO
        for (bytes_read = 0; bytes_read < size; bytes_read++)
        {
            while (rx_qsize() == 0) flush_read_fifo();
            data[bytes_read] = rx_dequeue();
        }
    }
    else if (timeout == 0)
    {
        // don't block
        for (bytes_read = 0; bytes_read < size; bytes_read++)
        {
            if (rx_qsize() == 0) break;
            data[bytes_read] = rx_dequeue();
        }
    }
    else // timeout > 0
    {
        // block till timeout, keep flushing FIFO
        reset_uart_timer();
        for (bytes_read = 0; bytes_read < size; bytes_read++)
        {
            while ((rx_qsize() == 0) && (uart_time() <= timeout)) flush_read_fifo();
            if (uart_time() > timeout) break;
            data[bytes_read] = rx_dequeue();
        }
    }

    if (rx_qsize() == 0) SET_RXLED(0);

    // Re-enable UART receive interrupt
    ROM_UARTIntEnable(UART_BASE, UART_INT_RX);

    return bytes_read;
}


// timeout in milliseconds
// timeout 0 means timeout right away
// negative timeout means wait forever
uint32_t uart_write(const void *vdata, uint32_t size, int32_t timeout)
{
    const uint8_t *data = (const uint8_t *)vdata;
    uint32_t bytes_written = 0;

    SET_TXLED(1);

    // enqueue all the data until timeout
    if (timeout < 0)
    {
        // block forever (until there is space in queue)
        for (bytes_written = 0; bytes_written < size; bytes_written++)
        {
            // wait for an interrupt to make room in the buffer
            while (tx_qsize() >= WRITE_BUFF_SIZE - 1) ROM_SysCtlSleep();
            tx_enqueue(data[bytes_written]);
        }
    }
    else if (timeout == 0)
    {
        // don't block
        for (bytes_written = 0; bytes_written < size; bytes_written++)
        {
            if (tx_qsize() >= WRITE_BUFF_SIZE - 1) break;
            tx_enqueue(data[bytes_written]);
        }
    }
    else // timeout > 0
    {
        // block till timeout
        reset_uart_timer();
        for (bytes_written = 0; bytes_written < size; bytes_written++)
        {
            // busy wait is bad, but I'm lazy and don't care about
            // power consumption in this application
            while ((tx_qsize() >= WRITE_BUFF_SIZE - 1) && (uart_time() <= timeout));
            if (uart_time() > timeout) break;
            tx_enqueue(data[bytes_written]);
        }
    }

    // restart sending of data if not currently underway
    flush_write_fifo();

    return bytes_written;
}


uint32_t uart_write_unbuffered(const void *vdata, uint32_t size)
{
    const uint8_t *data = (const uint8_t *)vdata;
    uint32_t bytes_written;

    // wait for the FIFO to empty
    while (tx_qsize());

    // send the data
    for (bytes_written = 0; bytes_written < size; bytes_written++)
    {
        while (!ROM_UARTSpaceAvail(UART_BASE));
        ROM_UARTCharPut(UART_BASE, data[bytes_written]);
    }

    return bytes_written;
}


// clears the rx overflow flag and returns the prior status of the
// overflow flag
bool uart_clear_overflow(void)
{
    bool status = rb_overflow;
    rb_overflow = false;
    return status;
}
