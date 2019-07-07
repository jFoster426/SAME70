#include "nrf.h"

uint8_t nrf_tx_buf_size = 0;

#ifdef __DEBUG__
// used for debugging only
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
#endif

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
    // apply register mask
    reg &= NRF_R_REGISTER_MASK;
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    // send the register we want to read from
    spi_transfer_byte(SPI0, reg | NRF_R_REGISTER);
    // create a pointer to the start of the buffer array
    uint8_t result = spi_transfer_byte(SPI0, 0x00);
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    // return the result
    return result;
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

uint8_t nrf_rx_pipe_size(uint8_t pipe_num)
{
    // catch for actually writing register vales, or just pipe number
    if (pipe_num < NRF_REG_RX_PW_P0) pipe_num += NRF_REG_RX_PW_P0;
    // read the register and return the value
    return nrf_read_reg_single(pipe_num);
}

uint8_t nrf_read_rx_payload(uint8_t pipe_num)
{
    // returns the index to where the first data received is in
    // nrf_rx_buf
    uint8_t pipe_buf_size = nrf_rx_pipe_size(pipe_num);
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    spi_transfer_byte(SPI0, 0b01100001);
    for (uint8_t i = 0; i < pipe_buf_size; i++)
    {
        nrf_rx_buf[nrf_rx_buf_idx++] = spi_transfer_byte(SPI0, 0xFF);
    }
    // deselect the NRF
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    return nrf_rx_buf_idx - pipe_buf_size;
}

void nrf_write_tx_payload(uint8_t *payload, uint8_t size)
{
    // activate chip select
    gpio_clr(NRF_CSN_PORT, NRF_CSN_PIN);
    spi_transfer_byte(SPI0, 0b10100000);
    for (uint8_t i = 0; i < size; i++) spi_transfer_byte(SPI0, payload[i]);
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

// these are more user friendly functions

void nrf_enable_autoack(uint8_t pipe)
{
    nrf_write_reg_single(NRF_REG_EN_AA, pipe & 0b00111111);
}

void nrf_set_mode(uint8_t mode)
{
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    reg_val &= ~(NRF_PRIM_RX);
    if (mode == NRF_MODE_RX) reg_val |= NRF_PRIM_RX;
    nrf_write_reg_single(NRF_REG_CONFIG, reg_val);
}

void nrf_stop_listening()
{
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    // do nothing if not in RX mode
    if (!(reg_val & NRF_PRIM_RX)) return;
    gpio_clr(NRF_CE_PORT, NRF_CE_PIN);
}

void nrf_start_listening()
{
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    // do nothing if not in RX mode
    if (!(reg_val & NRF_PRIM_RX)) return;
    gpio_set(NRF_CE_PORT, NRF_CE_PIN);
}

uint8_t nrf_conf()
{
    gpio_conf(NRF_CE_PORT, NRF_CE_PIN, PIO_OUTPUT);
    gpio_conf(NRF_CSN_PORT, NRF_CSN_PIN, PIO_OUTPUT);
    gpio_set(NRF_CSN_PORT, NRF_CSN_PIN);
    gpio_clr(NRF_CE_PORT, NRF_CE_PIN);
    for (uint32_t i = 0; i < 1000000; i++) asm volatile ("Nop"); // 100ms delay

    nrf_write_reg_single(NRF_REG_CONFIG, 0x0C);

    // enable auto-ack on all pipes
    nrf_enable_autoack(0b00111111);

    nrf_write_reg_single(NRF_REG_FEATURE, 0x00);
    nrf_write_reg_single(NRF_REG_DYNPD, 0x00);
    nrf_write_reg_single(NRF_REG_STATUS, NRF_STATUS_RX_DR | NRF_STATUS_TX_DS | NRF_STATUS_MAX_RT);
    nrf_flush_rx();
    nrf_flush_tx();

    // power up
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_CONFIG);
    reg_val |= NRF_PWR_UP;
    nrf_write_reg_single(NRF_REG_CONFIG, reg_val);
    for (uint32_t i = 0; i < 100000; i++); // delay 5ms
    nrf_set_mode(NRF_MODE_TX);
    // TODO: implement a check to see if there is an NRF present
    return 1;
}

void nrf_set_channel(uint8_t ch)
{
    ch &= 0b01111111;
    nrf_write_reg_single(NRF_REG_RF_CH, ch);
}

void nrf_set_data_rate(uint8_t rate)
{
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_RF_SETUP);
    reg_val &= ~(NRF_RF_DR_LOW | NRF_RF_DR_HIGH);
    switch (rate)
    {
        case NRF_DATARATE_250KBPS:
            reg_val |= NRF_RF_DR_LOW;
            break;
        case NRF_DATARATE_1MBPS:
            break;
        case NRF_DATARATE_2MBPS:
            reg_val |= NRF_RF_DR_HIGH;
            break;
        default:
            // unsupported data rate
            return;
    }
    nrf_write_reg_single(NRF_REG_RF_SETUP, reg_val);
}

void nrf_set_power_output(uint8_t power)
{
    uint8_t reg_val = nrf_read_reg_single(NRF_REG_RF_SETUP);
    reg_val &= 0b11111001;
    // don't overwrite other parts of the register
    reg_val |= (power & 0b110);
    nrf_write_reg_single(NRF_REG_RF_SETUP, reg_val);
}

void nrf_set_retries(uint8_t delay, uint8_t count)
{
    nrf_write_reg_single(NRF_REG_SETUP_RETR, (delay & 0xF0) | (count & 0x0F));
}

void nrf_open_writing_pipe(uint8_t *addr, uint8_t size, uint8_t buf_size)
{
    // enable the receiving pipe to receive the auto ack
    nrf_write_reg(NRF_REG_RX_ADDR_P0, &addr[0], size);
    nrf_write_reg(NRF_REG_TX_ADDR, &addr[0], size);
    nrf_write_reg_single(NRF_REG_RX_PW_P0, buf_size);
    nrf_tx_buf_size = buf_size;
}
void nrf_open_reading_pipe(uint8_t pipe_num, uint8_t *addr, uint8_t size, uint8_t buf_size)
{
    // check if the programmer typed 0 or NRF_REG_RX_ADDR_P0
    if (pipe_num < 0x0A) pipe_num += 0x0A;
    pipe_num &= 0b1111;
    // pipes 2-5 share the first 4 bytes
    if (pipe_num > 0x0B && size > 1) return;
    nrf_write_reg(pipe_num, &addr[0], size);
    // tell the size we expect to receive
    nrf_write_reg_single(pipe_num + 0x7, buf_size);
}

uint8_t nrf_send_packet(uint8_t *packet, uint8_t size)
{
    // define the maximum size we could possibly want
    uint8_t tx_packet_padded[32] = { 0 };
    for (int i = 0; i < size; i++) tx_packet_padded[i] = packet[i];
    // write the padded payload
    nrf_write_tx_payload(&tx_packet_padded[0], nrf_tx_buf_size);
    // set CE and wait for ack or max_rt
    gpio_set(NRF_CE_PORT, NRF_CE_PIN);
    uint8_t reg_val = 0;
    while (!(reg_val & (NRF_STATUS_TX_DS | NRF_STATUS_MAX_RT))) reg_val = nrf_read_status();
    gpio_clr(NRF_CE_PORT, NRF_CE_PIN);
    // return 0 if max_rt or anything but no ack received
    if (!(reg_val & NRF_STATUS_TX_DS))
    {
        reg_val = nrf_read_status();
        // write 1 to clear bit, enable further communication
        reg_val |= NRF_STATUS_MAX_RT;
        nrf_write_reg_single(NRF_REG_STATUS, reg_val);
        nrf_flush_tx();
        return 0;
    }
    // return 1 if ack was received, clear TX_DS
    reg_val = nrf_read_status();
    reg_val |= NRF_STATUS_TX_DS;
    nrf_write_reg_single(NRF_REG_STATUS, reg_val);
    return 1;
}

uint8_t nrf_which_pipe()
{
    // rx_p_no in status indicated which pipe data was received on
    uint8_t reg_val = nrf_read_status();
    return (reg_val >> 1) & 0b111;
    // note that this returns 0b111 when the RX FIFO is empty
}

uint8_t nrf_receive_packet(uint8_t *pos)
{
    // returns the pipe number of the message
    // takes as an argument a pointer that will return the index
    // of the first byte of data in the nrf_rx_buf
    // note this function is blocking
    while (!nrf_is_data_ready());
    uint8_t pipe_num = nrf_which_pipe();
    // set CE low
    nrf_stop_listening();

    // read data
    *pos = nrf_read_rx_payload(pipe_num);
    // clear rx_dr
    nrf_write_reg_single(NRF_REG_STATUS, NRF_STATUS_RX_DR);

    // set CE high
    nrf_start_listening();
    return pipe_num;
}

uint8_t nrf_is_data_ready()
{
    // return 1 if there is data ready in the RX FIFO
    uint8_t reg_val = nrf_read_status();
    if ((reg_val & NRF_STATUS_RX_DR) && (nrf_which_pipe() != 0b111)) return 1;
    return 0;
}
