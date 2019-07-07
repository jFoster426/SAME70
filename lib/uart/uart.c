#include "uart.h"
#include "../gpio/gpio.h"

#include <stdlib.h>

void uart_conf(Uart* uart_n, uint32_t baud, uint32_t parity)
{
    if (uart_n == UART0)
    {
        // enable the uart clock using the power management controller
        PMC->PMC_PCER0 |= PMC_PCER0_PID7;
        // disable the PIO controller (enables peripheral state control)
        gpio_conf(PIOA, PIO_PA9, PIO_DISABLE);
        gpio_conf(PIOA, PIO_PA10, PIO_DISABLE);
        // configure the gpio lines to the right state
        // uart0 pins (144 pin package):
        // RX - PA9 (PIO Alternate Function A)
        // TX - PA10 (PIO Alternate Function A)
        gpio_set_alt(PIOA, PIO_PA9,  PIO_ALTA);
        gpio_set_alt(PIOA, PIO_PA10, PIO_ALTA);
    }
    else if (uart_n == UART1)
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID8;
        gpio_conf(PIOA, PIO_PA5, PIO_DISABLE);
        gpio_conf(PIOA, PIO_PA6, PIO_DISABLE);
        // uart1 pins (144 pin package):
        // RX - PA5 (Alternate Function C)
        // TX - PA4, PA6 (Alternate Function C), PD26 (Alternate Function D)
        gpio_set_alt(PIOA, PIO_PA5, PIO_ALTC);
        gpio_set_alt(PIOA, PIO_PA6, PIO_ALTC);
    }
    else if (uart_n == UART2)
    {
        PMC->PMC_PCER1 |= PMC_PCER1_PID44;
        gpio_conf(PIOD, PIO_PD25, PIO_DISABLE);
        gpio_conf(PIOD, PIO_PD26, PIO_DISABLE);
        // uart2 pins (144 pin package):
        // RX - PD25 (Alternate Function C)
        // TX - PD26 (Alternate Function C)
        gpio_set_alt(PIOD, PIO_PD25, PIO_ALTC);
        gpio_set_alt(PIOD, PIO_PD26, PIO_ALTC);
    }
    else if (uart_n == UART3)
    {
        PMC->PMC_PCER1 |= PMC_PCER1_PID45;
        gpio_conf(PIOD, PIO_PD28, PIO_DISABLE);
        gpio_conf(PIOD, PIO_PD30, PIO_DISABLE);
        // uart2 pins (144 pin package):
        // RX - PD28 (Alternate Function A)
        // TX - PD30 (Alternate Function A), PD31 (Alternate Function B)
        gpio_set_alt(PIOD, PIO_PD28, PIO_ALTA);
        gpio_set_alt(PIOD, PIO_PD30, PIO_ALTA);
    }
    else if (uart_n == UART4)
    {
        PMC->PMC_PCER1 |= PMC_PCER1_PID46;
        gpio_conf(PIOD, PIO_PD18, PIO_DISABLE);
        gpio_conf(PIOD, PIO_PD19, PIO_DISABLE);
        // uart2 pins (144 pin package):
        // RX - PD18 (Alternate Function C)
        // TX - PD19 (Alternate Function C)
        gpio_set_alt(PIOD, PIO_PD18, PIO_ALTC);
        gpio_set_alt(PIOD, PIO_PD19, PIO_ALTC);
    }
    // change the parity to the user selected type
    // ensure a user error won't accidentally overwrite other critical bits
    uart_n->UART_MR |= ((parity << 9) & (7 << 9));
    // for now, disable all the interrupts
    uart_n->UART_IDR = 0;
    // set the baud rate, use the formula:
    // UART_BRGR = PCLK / (16 * baudrate) (between 1 and 65535)
    // the PCLK (unless otherwise changed)
    // F_PCLK is defined in the Makefile, and matches the configuration in startup_same70.c
    uart_n->UART_BRGR = F_PCLK / (16 * baud);
    // enable the uart
    uart_n->UART_CR &= ~(UART_CR_TXDIS | UART_CR_RXDIS);
    uart_n->UART_CR |=   UART_CR_TXEN  | UART_CR_RXEN;
    // wait until the tx module is ready to transmit
    while (!(uart_n->UART_SR & UART_SR_TXRDY));
}

void uart_enable_interrupt(Uart* uart_n, void* interruptHandler)
{
    // get rid of compiler warnings (temporary)
    uart_n += 1;
    interruptHandler += 1;
}

void uart_transmit_byte(Uart* uart_n, uint8_t txChar)
{
    // wait to be ready for another character transmit
    while (!(uart_n->UART_SR & UART_SR_TXRDY));
    // send the character to the tx buffer register
    uart_n->UART_THR = txChar;
}

void uart_transmit_string(Uart* uart_n, char* s)
{
    // send the characters until we hit the null character at the end
    while (*s) uart_transmit_byte(uart_n, *s++);
}

void uart_transmit_binary(Uart* uart_n, uint32_t b, uint8_t size)
{
    // function call looks like this:
    // uart_transmit_binary(UART0, 0b10101010, sizeof(uint8_t));
    // convert number of bytes to number of bits
    size *= 8;
    int i;
    uart_transmit_string(uart_n, "0b");
    for (i = size-1; i >= 0; i--)
    {
        if (b & (1 << i)) uart_transmit_byte(uart_n, '1');
        else              uart_transmit_byte(uart_n, '0');
    }
}

void uart_transmit_hex(Uart* uart_n, uint32_t h, uint8_t size)
{
    // function call looks like this:
    // uart_transmit_hex(UART0, 0x1000, sizeof(uint16_t));
    // convert number of bytes to number of bits

    // TO DO - implement this algorithm
    char base[20];
    itoa(h, base, 16);

    uart_transmit_string(uart_n, base);
    // suppress compiler warnings
    size++;
}

uint8_t uart_receive_byte(Uart* uart_n)
{
    // wait for data to be available for read
    while (!(uart_n->UART_SR & UART_SR_RXRDY));
    // get the data, this will automatically reset UART_SR_RXRDY, or transfer in the next byte
    return uart_n->UART_RHR & 255;
}
