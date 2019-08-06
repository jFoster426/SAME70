#include "same70q21.h"
#include "gpio/gpio.h"
#include "pwm/pwm.h"
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf/nrf.h"
#include "movement/movement.h"

volatile int8_t state = 0;
volatile uint8_t ledstate = 0;

// declare the motors we want
motor_t motor_FL = {MOTOR_FRONT_LEFT, PIOC, PIO_PC2, PIOA, PIO_PA0, PIO_ALTA, 0, 0, 0};   // M1
motor_t motor_FR = {MOTOR_FRONT_RIGHT, PIOC, PIO_PC8, PIOA, PIO_PA12, PIO_ALTB, 1, 0, 0}; // M2
motor_t motor_BL = {MOTOR_BACK_LEFT, PIOC, PIO_PC16, PIOA, PIO_PA25, PIO_ALTB, 2, 0, 0};  // M3
motor_t motor_BR = {MOTOR_BACK_RIGHT, PIOC, PIO_PC22, PIOC, PIO_PC13, PIO_ALTB, 3, 0, 0}; // M4

// a pointer array to point to all the motors on our robot
motor_t *motors[] = {&motor_FL, &motor_BL, &motor_FR, &motor_BR};

void irq_handler_tc0(void)
{
    if (TC0->TC_CHANNEL[0].TC_SR & TC_SR_CPCS)
    {
        motors_write_direction(motors, 0, 0, 0); // stop the motors
        gpio_write(PIOB, PIO_PB0, ledstate = ~ledstate);
        gpio_clr(PIOB, PIO_PB1);
    }
}

static void timer_conf(void)
{
  PMC->PMC_PCER0 = PMC_PCER0_PID23;

  TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG;
  TC0->TC_CHANNEL[0].TC_RC = 20000;
  TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

  NVIC_EnableIRQ(TC0_IRQn);
}

// PIOB0, PIOB1, PIOB2, PIOB3 are LED's
int main(void)
{
    // patch wire on demo board if you want this to work
    gpio_conf(PIOB, PIO_PB0, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB1, PIO_OUTPUT);

    // initialize all the motors
    motors_conf(motors);

    timer_conf();

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

        while (!nrf_is_data_ready());
        gpio_set(PIOB, PIO_PB1);

        uint8_t pos = 0;
        uint8_t pipe_num = nrf_receive_packet(&pos);
        uint8_t size_of_packet = nrf_rx_pipe_size(pipe_num);

        // timer interrupt turns off the motors for us
        if (nrf_rx_buf[pos] == 'W')
        {
            motors_write_direction(motors, 20, 0, 0);
        }
        else
        if (nrf_rx_buf[pos] == 'A')
        {
            motors_write_direction(motors, 20, -90, 0);
        }
        else
        if (nrf_rx_buf[pos] == 'S')
        {
            motors_write_direction(motors, 20, 180, 0);
        }
        else
        if (nrf_rx_buf[pos] == 'D')
        {
            motors_write_direction(motors, 20, 90, 0);
        }
        else
        if (nrf_rx_buf[pos] == 'Q')
        {
            motors_write_direction(motors, 0, 0, -10);
        }
        else
        if (nrf_rx_buf[pos] == 'E')
        {
            motors_write_direction(motors, 0, 0, 10);
        }

        TC0->TC_CHANNEL[0].TC_CCR |= TC_CCR_SWTRG; // resets counter for full pulse

    }

    return 0;
}
