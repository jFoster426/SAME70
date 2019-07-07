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
    nrf_set_channel(0x1F);
    nrf_set_data_rate(NRF_DATARATE_1MBPS);
    nrf_set_power_output(NRF_RF_PWR_0);
    nrf_set_retries(NRF_ARD_1000US, NRF_ARC_RETR_5);

    uint8_t addr[5] = {0, 1, 2, 3, 4};
    nrf_open_writing_pipe(&addr[0], 5, 32);

    uint8_t payload[] = "Message test.";

    while (1)
    {
        nrf_show_upto(0x07);
        if (nrf_send_packet(&payload[0], sizeof(payload)))
        {
            // ack was received
            gpio_set(PIOB, PIO_PB0);
            gpio_set(PIOB, PIO_PB1);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
            gpio_clr(PIOB, PIO_PB0);
            gpio_clr(PIOB, PIO_PB1);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
        }
        else
        {
            // max_rt timeout
            gpio_set(PIOB, PIO_PB2);
            gpio_set(PIOB, PIO_PB3);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
            gpio_clr(PIOB, PIO_PB2);
            gpio_clr(PIOB, PIO_PB3);
            for (volatile uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
        }
    }

    return 0;
}
