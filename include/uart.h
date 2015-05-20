// Software-buffered UART functions for Tiva C series microcontrollers
// Copyright (C) 2015 Sultan Qasim Khan

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init(int32_t baud_rate);
uint32_t uart_read(void *data, uint32_t size, int32_t timeout);
uint32_t uart_write(const void *data, uint32_t size, int32_t timeout);
uint32_t uart_write_unbuffered(const void *vdata, uint32_t size);
bool uart_clear_overflow(void);

#endif
