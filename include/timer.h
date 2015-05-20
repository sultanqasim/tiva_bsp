// Timer utility functions for Tiva C series
// Copyright (C) 2015 Sultan Qasim Khan

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Call this before using timer functions
void timer_init(void);

// Delay
void delay(uint32_t nTime);
void delayMicroseconds(uint32_t nTime);

// Timer
void resetTimer(void);
uint32_t millis(void);
uint32_t micros(void);

#endif
