// UART Command Defintions for Tiva BSP
// Copyright (C) 2015 Sultan Qasim Khan

#ifndef COMMANDS_H
#define COMMANDS_H

// Indexing starts as 1 to catch errors from data consisting of only zeros
enum Commands
{
    CMD_EXAMPLE = 0x01,
    CMD_SETGPIO = 0x02,
    CMD_NAK = 0xFE
};

#endif
