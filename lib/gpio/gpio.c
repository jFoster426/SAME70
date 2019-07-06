#include "gpio.h"

void gpio_conf(Pio *port, uint32_t pin, uint32_t dir)
{
    if (port == PIOA && !(PMC->PMC_PCSR0 & PMC_PCSR0_PID10))
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID10;
    }
    else if (port == PIOB && !(PMC->PMC_PCSR0 & PMC_PCSR0_PID11))
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID11;
        // take care because PIOB has some special pins (JTAG, ERASE)
        if (pin <= PIO_PB12) MATRIX->CCFG_SYSIO |= pin;
    }
    else if (port == PIOC && !(PMC->PMC_PCSR0 & PMC_PCSR0_PID12))
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID12;
    }
    else if (port == PIOD && !(PMC->PMC_PCSR0 & PMC_PCSR0_PID16))
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID16;
    }
    else if (port == PIOE && !(PMC->PMC_PCSR0 & PMC_PCSR0_PID17))
    {
        PMC->PMC_PCER0 |= PMC_PCER0_PID17;
    }

    if (dir & PIO_INPUT)
    {
        port->PIO_PER |= pin;
        port->PIO_ODR |= pin;
    }
    else if (dir & PIO_OUTPUT)
    {
        port->PIO_PER |= pin;
        port->PIO_OER |= pin;
        // enable writing to this pin
        port->PIO_OWER |= pin;
    }
    else if (dir & PIO_DISABLE)
    {
        port->PIO_PDR |= pin;
    }

    if (dir & PIO_INPUT_PULLUP)
    {
        port->PIO_PPDDR |= pin;
        port->PIO_PUER |= pin;
    }
    else if (dir & PIO_INPUT_PULLDOWN)
    {
        port->PIO_PUDR |= pin;
        port->PIO_PPDER |= pin;
    }
    else if (dir & PIO_INPUT_PULLX_DISABLE)
    {
        port->PIO_PUDR |= pin;
        port->PIO_PPDDR |= pin;
    }

    if (dir & PIO_INPUT_SCHMITT)
    {
        port->PIO_PER |= pin;
        port->PIO_ODR |= pin;
        port->PIO_SCHMITT |= pin;
    }
    else if (dir & PIO_INPUT_SCHMITT_DISABLE)
    {
        port->PIO_SCHMITT &= ~(pin);
    }

    if (dir & PIO_INPUT_FILTER)
    {
        port->PIO_PER |= pin;
        port->PIO_ODR |= pin;
        port->PIO_IFER |= pin;
    }
    else if (dir & PIO_INPUT_FILTER_DISABLE)
    {
        port->PIO_IFER |= pin;
    }
}

void gpio_set(Pio *port, uint32_t pin)
{
    port->PIO_SODR = pin;
}

void gpio_clr(Pio *port, uint32_t pin)
{
    port->PIO_CODR = pin;
}

void gpio_write(Pio *port, uint32_t pin, uint8_t val)
{
    if (val) gpio_set(port, pin);
    else     gpio_clr(port, pin);
}

uint8_t gpio_read(Pio *port, uint32_t pin)
{
    if (port->PIO_PDSR & pin) return 1;
    return 0;
}

void gpio_set_alt(Pio *port, uint32_t pin, uint8_t state)
{
    gpio_conf(port, pin, PIO_DISABLE);
    if (state & 1) port->PIO_ABCDSR[0] |=   pin;
    else           port->PIO_ABCDSR[0] &= ~(pin);
    if (state & 2) port->PIO_ABCDSR[1] |=   pin;
    else           port->PIO_ABCDSR[1] &= ~(pin);
}
