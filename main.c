#include "same70q21.h"
#include "gpio/gpio.h"
#include "pwm/pwm.h"
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf/nrf.h"
#include "movement/movement.h"

volatile int8_t state = 1;
volatile uint8_t ledstate = 0;

void irq_handler_tc0(void)
{
    if (TC0->TC_CHANNEL[0].TC_SR & TC_SR_CPCS)
    {
        gpio_write(PIOB, PIO_PB0, ledstate = ~ledstate);
    }
}

static void timer_conf(void)
{
  PMC->PMC_PCER0 = PMC_PCER0_PID23;

  TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG;
  TC0->TC_CHANNEL[0].TC_RC = 40000;
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
    motors_conf(4, motor_FL, motor_FR, motor_BL, motor_BR);

    timer_conf();

    while (1);

    return 0;
}
