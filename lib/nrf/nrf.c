#include "nrf.h"

uint8_t *nrf_read_reg(uint8_t reg, uint8_t size)
{
    // apply register mask
    reg &= NRF_R_REGISTER_MASK;
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send the register we want to read from
    spi_transfer_byte(SPI0, reg | NRF_R_REGISTER);
    // create a pointer to the start of the buffer array
    uint8_t *start_of_buffer = &nrf_rx_buf[nrf_rx_buf_idx];
    // implement circular buffer with j, since j is 8 bits and nrf_rx_buf
    // is 256 bytes long, overflow will result in index going back to 0
    for (uint8_t i = 0; i < size; i++) nrf_rx_buf[nrf_rx_buf_idx++] = spi_transfer_byte(SPI0, 0x00);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    // return the pointer to the start address of the buffer
    return start_of_buffer;
}

uint8_t nrf_read_reg_single(uint8_t reg)
{
    return *nrf_read_reg(reg, 1);
}

void nrf_write_reg(uint8_t reg, uint8_t *nrf_tx_buf, uint8_t size)
{
    // apply register mask
    reg &= NRF_W_REGISTER_MASK;
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send the register we want to write to
    spi_transfer_byte(SPI0, reg | NRF_W_REGISTER);
    // repeat for however many bytes we want to send
    for (uint8_t i = 0; i < size; i++) spi_transfer_byte(SPI0, *nrf_tx_buf++);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}

void nrf_write_reg_single(uint8_t reg, uint8_t nrf_tx_buf)
{
    // apply register mask
    reg &= NRF_W_REGISTER_MASK;
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send the register we want to write to
    spi_transfer_byte(SPI0, reg | NRF_W_REGISTER);
    // send a single byte
    spi_transfer_byte(SPI0, nrf_tx_buf);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}

uint8_t nrf_read_status()
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send anything while reading information
    uint8_t result = spi_transfer_byte(SPI0, 0xFF);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    return result;
}

void nrf_write_cmd(uint8_t cmd)
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send anything while reading information
    spi_transfer_byte(SPI0, cmd);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}

uint8_t nrf_read_rx_payload(uint8_t size)
{
    return size;
}

void nrf_write_tx_payload(uint8_t *payload, uint8_t size)
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    spi_transfer_byte(SPI0, 0b10100000);
    uint8_t i;
    for (i = 0; i < size; i++) spi_transfer_byte(SPI0, payload[i]);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}

void nrf_flush_tx()
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send anything while reading information
    spi_transfer_byte(SPI0, 0b11100001);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}

void nrf_flush_rx()
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send anything while reading information
    spi_transfer_byte(SPI0, 0b11100010);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
}
