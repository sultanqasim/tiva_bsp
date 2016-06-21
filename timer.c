// Timer utility functions for Tiva C series
// Copyright (C) 2015-2016 Sultan Qasim Khan

#include <timer.h>
#include <fwlibs.h>

void delay_isr(void);

void timer_init(void)
{
    // Run clock at 80 MHz using PLL and crystal
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Configure TIMER4 for timer functions
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER4);
    ROM_TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerEnable(TIMER4_BASE, TIMER_A);

    // Configure TIMER5 for delay functions
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER5);
    ROM_TimerConfigure(TIMER5_BASE, TIMER_CFG_ONE_SHOT);
    ROM_TimerDisable(TIMER5_BASE, TIMER_A);
    TimerIntRegister(TIMER5_BASE, TIMER_A, delay_isr);
    ROM_IntMasterEnable();
}


void delay_isr(void)
{
    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
}


void delay(uint32_t nTime)
{
    uint32_t etime = nTime * MS_CYCLES;

    // Since timer is one shot, we enable and disable it each time
    ROM_TimerEnable(TIMER5_BASE, TIMER_A);
    ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerLoadSet(TIMER5_BASE, TIMER_A, etime);
    ROM_SysCtlSleep();

    // Make sure the timer finished, go back to sleep if it didn't
    // (Another interrupt may have woken up the system)
    while (ROM_TimerValueGet(TIMER5_BASE, TIMER_A) != etime)
        ROM_SysCtlSleep();

    ROM_TimerIntDisable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerDisable(TIMER5_BASE, TIMER_A);
}


void delayMicroseconds(uint32_t nTime)
{
    if (!nTime) return;

    uint32_t etime = ROM_TimerValueGet(TIMER4_BASE, TIMER_A) - (nTime * US_CYCLES);
    while (ROM_TimerValueGet(TIMER4_BASE, TIMER_A) > etime);
}


// Note that the timer overflows after 35 seconds
// Thus, it is recommended that one reset the timer before use
void resetTimer(void)
{
    ROM_TimerLoadSet(TIMER4_BASE, TIMER_A, 0xFFFFFFFF);
}


uint32_t millis(void)
{
    return (0xFFFFFFFF - ROM_TimerValueGet(TIMER4_BASE, TIMER_A)) / MS_CYCLES;
}


uint32_t micros(void)
{
    return (0xFFFFFFFF - ROM_TimerValueGet(TIMER4_BASE, TIMER_A)) / US_CYCLES;
}
