#include "same70q21.h"
#include "../gpio/gpio.h"
#include "../spi/spi.h"

#ifndef NRF_H
#define NRF_H

#define NRF_CE_PORT   PIOD
#define NRF_CSN_PORT  PIOD
#define NEF_IRQ_PORT
#define NRF_CE_PIN    PIO_PD25
#define NRF_CSN_PIN   PIO_PD26
#define NRF_IRQ_PIN

#define NRF_R_REGISTER           (0b00000000)
#define NRF_R_REGISTER_MASK      (0b00011111)
#define NRF_W_REGISTER           (0b00100000)
#define NRF_W_REGISTER_MASK      (0b00011111)
#define NRF_R_RX_PAYLOAD         (0b01100001)
#define NRF_W_TX_PAYLOAD         (0b10100000)
#define NRF_W_ACK_PAYLOAD        (0b10101000)
#define NRF_ACK_PAYLOAD_PIPE_0   (0b00000000)
#define NRF_ACK_PAYLOAD_PIPE_1   (0b00000001)
#define NRF_ACK_PAYLOAD_PIPE_2   (0b00000010)
#define NRF_ACK_PAYLOAD_PIPE_3   (0b00000011)
#define NRF_ACK_PAYLOAD_PIPE_4   (0b00000100)
#define NRF_ACK_PAYLOAD_PIPE_5   (0b00000101)
#define NRF_W_TX_PAYLOAD_NO_ACK  (0b10110000)
#define NRF_FLUSH_TX             (0b11100001)
#define NRF_FLUSH_RX             (0b11100010)
#define NRF_REUSE_TX_PL          (0b11100011)
#define NRF_R_RX_PL_WID          (0b01100000)
#define NRF_NOP                  (0b11111111)

#define NRF_REG_CONFIG        0x00
#define NRF_MASK_RX_DR      (1 << 6)
#define NRF_MASK_TX_DS      (1 << 5)
#define NRF_MASK_MAX_RT     (1 << 4)
#define NRF_EN_CRC          (1 << 3)
#define NRF_CRCO            (1 << 2)
#define NRF_PWR_UP          (1 << 1)
#define NRF_PRIM_RX         (1 << 0)

#define NRF_REG_EN_AA         0x01
#define NRF_ENAA_P5         (1 << 5)
#define NRF_ENAA_P4         (1 << 4)
#define NRF_ENAA_P3         (1 << 3)
#define NRF_ENAA_P2         (1 << 2)
#define NRF_ENAA_P1         (1 << 1)
#define NRF_ENAA_P0         (1 << 0)

#define NRF_REG_EN_RXADDR     0x02
#define NRF_ERX_P5          (1 << 5)
#define NRF_ERX_P4          (1 << 4)
#define NRF_ERX_P3          (1 << 3)
#define NRF_ERX_P2          (1 << 2)
#define NRF_ERX_P1          (1 << 1)
#define NRF_ERX_P0          (1 << 0)

#define NRF_REG_SETUP_AW      0x03
#define NRF_AW              (1 << 0)

#define NRF_REG_SETUP_RETR    0x04
#define NRF_ARD_250US     (0b0000 << 4)
#define NRF_ARD_500US     (0b0001 << 4)
#define NRF_ARD_750US     (0b0010 << 4)
#define NRF_ARD_1000US    (0b0011 << 4)
#define NRF_ARD_1250US    (0b0100 << 4)
#define NRF_ARD_1500US    (0b0101 << 4)
#define NRF_ARD_1750US    (0b0110 << 4)
#define NRF_ARD_2000US    (0b0111 << 4)
#define NRF_ARD_2250US    (0b1000 << 4)
#define NRF_ARD_2500US    (0b1001 << 4)
#define NRF_ARD_2750US    (0b1010 << 4)
#define NRF_ARD_3000US    (0b1011 << 4)
#define NRF_ARD_3250US    (0b1100 << 4)
#define NRF_ARD_3500US    (0b1110 << 4)
#define NRF_ARD_3750US    (0b1110 << 4)
#define NRF_ARD_4000US    (0b1111 << 4)
#define NRF_ARC_RETR_DIS      0b0000
#define NRF_ARC_RETR_1        0b0001
#define NRF_ARC_RETR_2        0b0010
#define NRF_ARC_RETR_3        0b0011
#define NRF_ARC_RETR_4        0b0100
#define NRF_ARC_RETR_5        0b0101
#define NRF_ARC_RETR_6        0b0110
#define NRF_ARC_RETR_7        0b0111
#define NRF_ARC_RETR_8        0b1000
#define NRF_ARC_RETR_9        0b1001
#define NRF_ARC_RETR_10       0b1010
#define NRF_ARC_RETR_11       0b1011
#define NRF_ARC_RETR_12       0b1100
#define NRF_ARC_RETR_13       0b1101
#define NRF_ARC_RETR_14       0b1110
#define NRF_ARC_RETR_15       0b1111

#define NRF_REG_RF_CH         0x05
#define NRF_RF_CH           (1 << 0)

#define NRF_REG_RF_SETUP      0x06
#define NRF_CONT_WAVE       (1 << 7)
#define NRF_RF_DR_LOW       (1 << 5)
#define NRF_PLL_LOCK        (1 << 4)
#define NRF_RF_DR_HIGH      (1 << 3)
#define NRF_RF_PWR_M18   (0b00 << 1)
#define NRF_RF_PWR_M12   (0b01 << 1)
#define NRF_RF_PWR_M6    (0b10 << 1)
#define NRF_RF_PWR_0     (0b11 << 1)


#define NRF_REG_STATUS        0x07
#define NRF_STATUS_RX_DR    (1 << 6)
#define NRF_STATUS_TX_DS    (1 << 5)
#define NRF_STATUS_MAX_RT   (1 << 4)
#define NRF_STATUS_RX_P_NO  (1 << 1)
#define NRF_STATUS_TX_FULL  (1 << 0)

#define NRF_REG_OBSERVE_TX    0x08
#define NRF_PLOS_CNT        (1 << 4)
#define NRF_ARC_CNT         (1 << 0)

#define NRF_REG_RPD           0x09
#define NRF_RPD             (1 << 0)

#define NRF_REG_RX_ADDR_P0    0x0A
#define NRF_REG_RX_ADDR_P1    0x0B
#define NRF_REG_RX_ADDR_P2    0x0C
#define NRF_REG_RX_ADDR_P3    0x0D
#define NRF_REG_RX_ADDR_P4    0x0E
#define NRF_REG_RX_ADDR_P5    0x0F

#define NRF_REG_TX_ADDR       0x10

#define NRF_REG_RX_PW_P0      0x11
#define NRF_REG_RX_PW_P1      0x12
#define NRF_REG_RX_PW_P2      0x13
#define NRF_REG_RX_PW_P3      0x14
#define NRF_REG_RX_PW_P4      0x15
#define NRF_REG_RX_PW_P5      0x16

#define NRF_REG_FIFO_STATUS   0x17
#define NRF_TX_REUSE        (1 << 6)
#define NRF_FIFO_TX_FULL    (1 << 5)
#define NRF_FIFO_TX_EMPTY   (1 << 4)
#define NRF_FIFO_RX_FULL    (1 << 1)
#define NRF_FIFO_RX_EMPTY   (1 << 0)

#define NRF_REG_DYNPD         0x1C
#define NRF_DPL_P5          (1 << 5)
#define NRF_DPL_P4          (1 << 4)
#define NRF_DPL_P3          (1 << 3)
#define NRF_DPL_P2          (1 << 2)
#define NRF_DPL_P1          (1 << 1)
#define NRF_DPL_P0          (1 << 0)

#define NRF_REG_FEATURE       0x1D
#define NRF_EN_DPL          (1 << 2)
#define NRF_EN_ACK_PAY      (1 << 1)
#define NED_EN_DYN_ACK      (1 << 0)

uint8_t nrf_rx_buf[256];
static volatile uint8_t nrf_rx_buf_idx = 0;

uint8_t nrf_read_reg_single(uint8_t reg);
uint8_t *nrf_read_reg(uint8_t reg, uint8_t size);

void nrf_write_reg_single(uint8_t reg, uint8_t nrf_tx_buf);
void nrf_write_reg(uint8_t reg, uint8_t *nrf_tx_buf, uint8_t size);

uint8_t nrf_read_status();

void nrf_write_cmd(uint8_t cmd);

uint8_t nrf_read_rx_payload(uint8_t size);

void nrf_write_tx_payload(uint8_t payload[], uint8_t size);

void nrf_flush_tx();

void nrf_flush_rx();

#endif
