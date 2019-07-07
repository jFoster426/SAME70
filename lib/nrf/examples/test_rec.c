#include <stdio.h>
#include <stdlib.h>
#include "same70q21.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf/nrf.h"

// PIOB0, PIOB1, PIOB2, PIOB3 are LED's
int main(void)
{
    gpio_conf(PIOB, PIO_PB0, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB1, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB2, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB3, PIO_OUTPUT);

    uart_conf(UART0, 115200, UART_NO_PARITY);

    // configure SPI0
    spi_conf(SPI0, 1000000UL, SPI_MODE_0);

    nrf_conf();
    nrf_set_mode(NRF_MODE_RX);
    nrf_set_channel(0x1F);
    nrf_set_data_rate(NRF_DATARATE_1MBPS);
    nrf_set_power_output(NRF_RF_PWR_0);

    uint8_t addr[5] = {0, 1, 2, 3, 4};
    // last parameter is how many bytes the packet is
    nrf_open_reading_pipe(NRF_PIPE_1, &addr[0], 5, 1);

    nrf_start_listening();

    while (1)
    {
        while (!nrf_is_data_ready())
        {
            // max_rt timeout
            gpio_set(PIOB, PIO_PB2);
            gpio_set(PIOB, PIO_PB3);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
            gpio_clr(PIOB, PIO_PB2);
            gpio_clr(PIOB, PIO_PB3);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
        }
        uint8_t pos = 0;
        uint8_t pipe_num = nrf_receive_packet(&pos);
        uint8_t size_of_packet = nrf_rx_pipe_size(pipe_num);

        uart_transmit_string(UART0, "\nReceived: ");

        for (uint16_t i = 0; i < size_of_packet; i++)
        {
            uart_transmit_byte(UART0, nrf_rx_buf[pos++]);
        }
        // show we received without the terminal
        gpio_set(PIOB, PIO_PB0);
        gpio_set(PIOB, PIO_PB1);
        for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
        gpio_clr(PIOB, PIO_PB0);
        gpio_clr(PIOB, PIO_PB1);
        for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
    }

    return 0;
}
