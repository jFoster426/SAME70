#include "same70q21.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf/nrf.h"


void nrf_show_upto(uint8_t upto)
{
    uart_transmit_string(UART0, "\n\nNRF Register Dump:\n\n");
    uint8_t reg_val;
    // ***** Memory Dump *****
    for (uint8_t i = 0; i <= upto; i++)
    {
        reg_val = nrf_read_reg_single(i);
        uart_transmit_hex(UART0, i, 1);
        uart_transmit_string(UART0, ":   ");
        uart_transmit_binary(UART0, reg_val, 1);
        uart_transmit_string(UART0, "\n");
        if (i >= 0x17) break;
    }
    if (upto >= 0x1C)
    {
        reg_val = nrf_read_reg_single(0x1C);
        uart_transmit_hex(UART0, 0x1C, 1);
        uart_transmit_string(UART0, ":   ");
        uart_transmit_binary(UART0, reg_val, 1);
        uart_transmit_string(UART0, "\n");
    }
    if (upto >= 0x1D)
    {
        reg_val = nrf_read_reg_single(0x1D);
        uart_transmit_hex(UART0, 0x1D, 1);
        uart_transmit_string(UART0, ":   ");
        uart_transmit_binary(UART0, reg_val, 1);
        uart_transmit_string(UART0, "\n\n");
    }
}

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

    // ** should be inside function radio.begin()
    gpio_conf(NRF_CE_PORT, NRF_CE_PIN, PIO_OUTPUT);
    gpio_conf(NRF_CSN_PORT, NRF_CSN_PIN, PIO_OUTPUT);
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    gpio_clr(NRF_CE_PORT, NRF_CE_PIN);
    for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop"); // 100ms delay

    nrf_write_reg_single(NRF_REG_CONFIG, 0x0C);

    // enable auto-ack on all pipes
    nrf_write_reg_single(NRF_REG_EN_AA, 0b00111111);

    nrf_write_reg_single(NRF_REG_FEATURE, 0x00);
    nrf_write_reg_single(NRF_REG_DYNPD, 0x00);
    nrf_write_reg_single(NRF_REG_STATUS, NRF_STATUS_RX_DR | NRF_STATUS_TX_DS | NRF_STATUS_MAX_RT);
    nrf_flush_rx();
    nrf_flush_tx();

    // ** power up
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    reg_val |= NRF_PWR_UP;
    nrf_write_reg_single(NRF_REG_CONFIG, reg_val);
    for (uint32_t i = 0; i < 100000; i++); // delay 5ms
    reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    reg_val &= ~(NRF_PRIM_RX); // change to tx mode
    nrf_write_reg_single(NRF_REG_CONFIG, reg_val);
    // **

    // ** function radio.setChannel(int channel)
    nrf_write_reg_single(NRF_REG_RF_CH, 0x1F);
    // **

    // ** function radio.setDataRate(int rate)
    reg_val = nrf_read_reg_single(NRF_REG_RF_SETUP);
    // set 1 mbps
    reg_val &= ~(NRF_RF_DR_LOW | NRF_RF_DR_HIGH);
    // set highest power mode
    reg_val &= 0b11111001;
    reg_val |= NRF_RF_PWR_0;
    nrf_write_reg_single(NRF_REG_RF_SETUP, reg_val);
    // **

    // ** function radio.setRetries(int delay, int count)
    nrf_write_reg_single(NRF_REG_SETUP_RETR, NRF_ARD_1000US | NRF_ARC_RETR_5);
    // **

    // function radio.openWritingPipe(long int address)
    uint8_t addr[5] = {0, 1, 2, 3, 4};
    nrf_write_reg(NRF_REG_RX_ADDR_P0, &addr[0], 5);
    nrf_write_reg(NRF_REG_TX_ADDR, &addr[0], 5);
    nrf_write_reg_single(NRF_REG_RX_PW_P0, 32);
    // **

    uint8_t payload[32] =
    {
        'M','e','s','s','a','g','e',' ',
        '0', 0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    while (1)
    {
        nrf_write_tx_payload(&payload[0], 32);
        gpio_set(NRF_CE_PORT, NRF_CE_PIN);
        reg_val = 0;
        while (!(reg_val & (NRF_STATUS_TX_DS | NRF_STATUS_MAX_RT))) reg_val = nrf_read_status();
        gpio_clr(NRF_CE_PORT, NRF_CE_PIN);

        // nrf_show_upto(0x07);
        if (reg_val & NRF_STATUS_TX_DS)
        {
            // ack was received
            gpio_set(PIOB, PIO_PB0);
            gpio_set(PIOB, PIO_PB1);
            for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
            gpio_clr(PIOB, PIO_PB0);
            gpio_clr(PIOB, PIO_PB1);
            for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
        }
        if (reg_val & NRF_STATUS_MAX_RT)
        {
            // blink something always
            gpio_set(PIOB, PIO_PB2);
            gpio_set(PIOB, PIO_PB3);
            for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");
            gpio_clr(PIOB, PIO_PB2);
            gpio_clr(PIOB, PIO_PB3);
            for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop");

            reg_val = nrf_read_status();
            // write 1 to clear bit, enable further communication
            reg_val |= NRF_STATUS_MAX_RT;
            nrf_write_reg_single(NRF_REG_STATUS, reg_val);
            nrf_flush_tx();
        }
    }
    return 0;
}
