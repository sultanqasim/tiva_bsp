// Tiva BSP for TM4C129x
// Copyright (C) 2015 Sultan Qasim Khan

#ifndef FWLIBS_H
#define FWLIBS_H

// C standard library headers
#include <stdint.h>
#include <stdbool.h>

// Chip-specific headers and defines
#ifndef PART_TM4C1294NCPDT
#define PART_TM4C1294NCPDT
#define TARGET_IS_SNOWFLAKE_RA1
#endif
#include "inc/tm4c1294ncpdt.h"

// globally define clock speed
#define CLOCK_SPEED 120000000
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

#endif
