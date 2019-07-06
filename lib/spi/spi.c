#include "spi.h"

#include "../uart/uart.h"

void spi_conf(Spi *spi_n, uint32_t bitrate, uint8_t spi_mode)
{
    // enable the spi clock through the pmc controller
    if (spi_n == SPI0 && !(PMC->PMC_PCER0 & PMC_PCER0_PID21)) PMC->PMC_PCER0 |= PMC_PCER0_PID21;
    else if (spi_n == SPI1 && !(PMC->PMC_PCER1 & PMC_PCER1_PID42)) PMC->PMC_PCER1 |= PMC_PCER1_PID42;
    // good practice to reset everything so that we know what is being changed
    // also some registers need to be written to, even if we just want to use the default values
    //spi_n->SPI_CR = 0;
    spi_n->SPI_MR = 0;
    spi_n->SPI_CSR[0] = 0;
    spi_n->SPI_CSR[1] = 0;
    spi_n->SPI_CSR[2] = 0;
    spi_n->SPI_CSR[3] = 0;
    // disable all the interrupts
    spi_n->SPI_IDR = 0xff;
    // disable the SPI, and hardware reset it
    spi_n->SPI_CR = (SPI_CR_SPIDIS | SPI_CR_SWRST);
    // set the spi to master mode, disable mode fault protection
    spi_n->SPI_MR |= (SPI_MR_MSTR | SPI_MR_MODFDIS);
    // use the NPCS0 pin as the SS signal for the SPI
    spi_n->SPI_MR |= SPI_MR_PCS(0);
    // check to see if the programmer used the proper library definitions
    // otherwise things don't work properly (Mode 0 != 0)
    if (!(spi_mode & 8)) return;
    // set the spi mode
    spi_n->SPI_CSR[0] |= (spi_mode & 3);
    // configure the SPI to only transfer 8 bits per transfer
    spi_n->SPI_CSR[0] |= SPI_CSR_BITS_8_BIT;
    // set the bit rate for the SPI transfer
    // at 150MHz P_CLK, max bitrate is 150Mhz, min bitrate = 588kHz
    // do not allow bitrate = 0
    if (bitrate == 0) bitrate = 1;
    uint8_t csr_val = ((F_PCLK / bitrate) & 0xff);
    if (csr_val == 0) csr_val = 1;
    // update all the clocks just in case
    spi_n->SPI_CSR[0] |= csr_val << 8;
    spi_n->SPI_CSR[1] |= csr_val << 8;
    spi_n->SPI_CSR[2] |= csr_val << 8;
    spi_n->SPI_CSR[3] |= csr_val << 8;
    // configure the sclk pin to do spi
    // sclk on pd22 (default)
    gpio_conf(PIOD, PIO_PD22, PIO_DISABLE);
    gpio_set_alt(PIOD, PIO_PD22, PIO_ALTB);
    // configure the mosi pin to do spi
    // mosi on pd21 (default)
    gpio_conf(PIOD, PIO_PD21, PIO_DISABLE);
    gpio_set_alt(PIOD, PIO_PD21, PIO_ALTB);
    // configure the miso pin to do spi
    // miso on pd20 (default)
    gpio_conf(PIOD, PIO_PD20, PIO_DISABLE);
    gpio_set_alt(PIOD, PIO_PD20, PIO_ALTB);
    // enable the SPI trasnmit and receive
    spi_n->SPI_CR |= SPI_CR_SPIEN;
    // wait for the state to be reflected in the status register
    while(!(spi_n->SPI_SR & SPI_SR_SPIENS));
}

static void spi_send_byte(Spi *spi_n, uint8_t data)
{
    // wait until the previous byte in SPI_TDR has been transferred to the shift register
    // goes high when it is empty
    while(!(spi_n->SPI_SR & SPI_SR_TXEMPTY));
    // SPI_TDR is the transmit data buffer
    // it can hold a 16 bit number
    // make sure to select device 0 when writing
    spi_n->SPI_TDR = SPI_TDR_PCS(0) | data;
}

uint8_t spi_available(Spi *spi_n)
{
    // bit 0 is RDRF, = 1 if there is data in the RX buffer waiting to be read
    uint8_t dummy = spi_n->SPI_SR & SPI_SR_RDRF;
    return dummy;
}

static uint8_t spi_read_byte(Spi *spi_n)
{
    // SPI_RDR is the receive data register
    // it is a 16 bit value by default
    uint8_t dummy = spi_n->SPI_RDR & 0xff;
    return dummy;
}

// spi_transfer_byte should be the only function the user calls to transfer data, since:
// "Data cannot be loaded in SPI_RDR without transmitting data.
// If there is no data to transmit, dummy data can be used (SPI_TDR filled with ones)."

uint8_t spi_transfer_byte(Spi *spi_n, uint8_t data)
{
    spi_send_byte(spi_n, data);
    // right now this thing never returns
    while(!spi_available(spi_n));
    return spi_read_byte(spi_n);
}
