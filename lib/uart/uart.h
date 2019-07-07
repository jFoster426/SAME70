#ifndef UART_H
#define UART_H

#include "same70q21.h"

#define UART_EVEN_PARITY  0
#define UART_ODD_PARITY   1
#define UART_SPACE_PARITY 2
#define UART_MARK_PARITY  3
#define UART_NO_PARITY    4

void uart_conf(Uart* uart_n, uint32_t baud, uint32_t parity);

void uart_enable_interrupt(Uart* uart_n, void* interruptHandler);

void uart_transmit_byte(Uart* uart_n, uint8_t txChar);

void uart_transmit_string(Uart* uart_n, char* s);

void uart_transmit_binary(Uart* uart_n, uint32_t b, uint8_t size);

void uart_transmit_hex(Uart* uart_n, uint32_t h, uint8_t size);

uint8_t uart_receive_byte(Uart* uart_n);

#endif
