// Tiva BSP for TM4C123G
// Copyright (C) 2015-2016 Sultan Qasim Khan

#ifndef FWLIBS_H
#define FWLIBS_H

// C standard library headers
#include <stdint.h>
#include <stdbool.h>

// Chip-specific headers and defines
#ifndef PART_TM4C123GH6PM
#define PART_TM4C123GH6PM
#define TARGET_IS_BLIZZARD_RA1
#endif
#include "inc/tm4c123gh6pm.h"

// globally define clock speed
#define CLOCK_SPEED 80000000
#define MS_CYCLES (CLOCK_SPEED / 1000)
#define US_CYCLES (CLOCK_SPEED / 1000000)

// handy little functions useful in many places
static inline void SET_BIT(uint8_t *buff, uint32_t bit, bool value)
{
    if (value) buff[bit >> 3] |= 1 << (bit & 0x7);
    else buff[bit >> 3] &= ~(1 << (bit & 0x7));
}

static inline void TOGGLE_BIT(uint8_t *buff, uint32_t bit)
{
    buff[bit >> 3] ^= 1 << (bit & 0x7);
}

#define ARM_NOP() __asm volatile("mov r0,r0")
#define ARM_NOP2() __asm volatile("mov r0,r0\n\tmov r1,r1")

// Other hardware related headers (I may not need all of them)
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"

// It is useful to have NULL defined
#ifndef NULL
#define NULL ((void *)0)
#endif

// The new TM4C129x microcontrollers can have a lot of pins...
typedef enum PinNumber
{
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7,
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7,
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7,
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7,
    PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7,
    PJ0, PJ1, PJ2, PJ3, PJ4, PJ5, PJ6, PJ7,
    PK0, PK1, PK2, PK3, PK4, PK5, PK6, PK7,
    PL0, PL1, PL2, PL3, PL4, PL5, PL6, PL7,
    PM0, PM1, PM2, PM3, PM4, PM5, PM6, PM7,
    PN0, PN1, PN2, PN3, PN4, PN5, PN6, PN7,
    PP0, PP1, PP2, PP3, PP4, PP5, PP6, PP7,
    PQ0, PQ1, PQ2, PQ3, PQ4, PQ5, PQ6, PQ7
} PinNumber;

static const uint32_t SysCtlGPIOs[] =
{
    SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC,
    SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF
};

static const uint32_t GPIO[] =
{
    GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE, GPIO_PORTD_BASE,
    GPIO_PORTE_BASE, GPIO_PORTF_BASE
};

#endif
