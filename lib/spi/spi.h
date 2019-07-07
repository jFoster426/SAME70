#ifndef SPI_H
#define SPI_H

#include "same70q21.h"
#include "../gpio/gpio.h"

// note they don't map properly!
// the code will check if the user used these definitions or just typed '0'
// by and-ing with 8 and returning if not true
#define SPI_MODE_0 (2+8)
#define SPI_MODE_1 (0+8)
#define SPI_MODE_2 (3+8)
#define SPI_MODE_3 (1+8)

void spi_conf(Spi *spi_n, uint32_t bitrate, uint8_t spi_mode);

//static void spi_send_byte(Spi *spi_n, uint8_t data);

uint8_t spi_available(Spi *spi_n);

//static uint8_t spi_read_byte(Spi *spi_n);

uint8_t spi_transfer_byte(Spi *spi_n, uint8_t data);

#endif
