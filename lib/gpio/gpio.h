#include "same70q21.h"

#ifndef GPIO_H
#define GPIO_H

// allow for multiple I/O changes with only one function call
// e.g. gpio_conf(PIOA, PIO_PB0, PIO_INPUT | PIO_INPUT_PULLUP | PIO_INPUT_SCHMITT);
#define PIO_INPUT                   1
#define PIO_OUTPUT                  2
#define PIO_DISABLE                 4
#define PIO_INPUT_PULLUP            8
#define PIO_INPUT_PULLDOWN          16
#define PIO_INPUT_PULLX_DISABLE     32
#define PIO_INPUT_SCHMITT           64
#define PIO_INPUT_SCHMITT_DISABLE   128
#define PIO_INPUT_FILTER            256
#define PIO_INPUT_FILTER_DISABLE    512

#define PIO_ALTA 0
#define PIO_ALTB 1
#define PIO_ALTC 2
#define PIO_ALTD 3

// enable the gpio and configure a pin as input, output, or internal pullup
void gpio_conf(Pio *port, uint32_t pin, uint32_t dir);

// sets a pin high
void gpio_set(Pio *port, uint32_t pin);

// sets a pin low
void gpio_clr(Pio *port, uint32_t pin);

// write to the pin
void gpio_write(Pio *port, uint32_t pin, uint8_t val);

// reads the value of the pin, regardless of its state
uint8_t gpio_read(Pio *port, uint32_t pin);

// configure the alternate states of pins
void gpio_set_alt(Pio *port, uint32_t pin, uint8_t state);

#endif
