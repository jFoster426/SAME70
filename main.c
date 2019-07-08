//Motor direction
#define DIR_FRONT_RIGHT    PIO_PC2
#define DIR_BACK_RIGHT     PIO_PC8
#define DIR_BACK_LEFT      PIO_PC16
#define DIR_FRONT_LEFT     PIO_PC22

//Motor mode
#define MODE_FRONT_RIGHT    PIO_PC3
#define MODE_BACK_RIGHT     PIO_PC9
#define MODE_BACK_LEFT      PIO_PC17
#define MODE_FRONT_LEFT     PIO_PC23

//LEDs
#define LED_GREEN    IOPORT_CREATE_PIN(PIOB, 0)
#define LED_YELLOW    IOPORT_CREATE_PIN(PIOB, 1)
#define LED_RED1    IOPORT_CREATE_PIN(PIOB, 2)
#define LED_RED2    IOPORT_CREATE_PIN(PIOB, 3)

/** PWM MOTOR0 pin definitions.  Front right (M1)*/
#define MOTOR_FRONT_RIGHT_GPIO			PIO_PA0_IDX
#define MOTOR_FRONT_RIGHT_FLAGS			(IOPORT_MODE_MUX_A)
#define MOTOR_FRONT_RIGHT_CHANNEL		PWM_CHANNEL_0

/** PWM MOTOR1 pin definitions. Back right (M2)*/
#define MOTOR_BACK_RIGHT_GPIO			PIO_PA12_IDX
#define MOTOR_BACK_RIGHT_FLAGS			(IOPORT_MODE_MUX_B)
#define MOTOR_BACK_RIGHT_CHANNEL		PWM_CHANNEL_1

/** PWM MOTOR2 pin definitions. Back left (M3)*/
#define MOTOR_BACK_LEFT_GPIO			PIO_PA25_IDX
#define MOTOR_BACK_LEFT_FLAGS			(IOPORT_MODE_MUX_B)
#define MOTOR_BACK_LEFT_CHANNEL			PWM_CHANNEL_2

/** PWM MOTOR3 pin definitions. Front left (M4)*/
#define MOTOR_FRONT_LEFT_GPIO			PIO_PC13_IDX
#define MOTOR_FRONT_LEFT_FLAGS			(IOPORT_MODE_MUX_B)
#define MOTOR_FRONT_LEFT_CHANNEL		PWM_CHANNEL_3

#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>
#include "same70q21.h"
#include "gpio/gpio.h"
#include "pwm/pwm.h"
#include "uart/uart.h"
#include "spi/spi.h"
#include "nrf/nrf.h"

#define MOTOR_PERIOD_VALUE       8500			// This will set the frequency of the PWM to 17.6KHz (300MHz/(2*MOTOR_PERIOD VALUE))

static int dutyCycleMotorFrontRight = MOTOR_PERIOD_VALUE;			/* PWM duty cycle for MOTOR_FRONT_RIGHT */
static int dutyCycleMotorBackRight = MOTOR_PERIOD_VALUE;			/* PWM duty cycle for MOTOR_BACK_RIGHT*/
static int dutyCycleMotorBackLeft = MOTOR_PERIOD_VALUE;				/* PWM duty cycle for MOTOR_BACK_LEFT*/
static int dutyCycleMotorFrontLeft = MOTOR_PERIOD_VALUE;			/* PWM duty cycle for MOTOR_FRONT_LEFT*/

pwm_channel_t pwm_channel_0;

void motorsMovementSpeed(int frontRight, int backRight, int backLeft, int frontLeft);



//PWM Handlers							///////////////////////////////////////////////////
void irq_handler_pwm0(void)
{
	uint32_t events = pwm_channel_get_interrupt_status(PWM0);

	/* Interrupt on MOTOR_FRONT_RIGHT_CHANNEL */
	if ((events & (1 << MOTOR_FRONT_RIGHT_CHANNEL)) ==
	(1 << MOTOR_FRONT_RIGHT_CHANNEL)) {

		// Set the PWM for MOTOR_FRONT_RIGHT_CHANNEL
		pwm_channel_0.channel = MOTOR_FRONT_RIGHT_CHANNEL;
		pwm_channel_update_duty(PWM0, &pwm_channel_0, dutyCycleMotorFrontRight);

		// Set the PWM for MOTOR_BACK_RIGHT_CHANNEL
		pwm_channel_0.channel = MOTOR_BACK_RIGHT_CHANNEL;
		pwm_channel_update_duty(PWM0, &pwm_channel_0, dutyCycleMotorBackRight);

		// Set the PWM for MOTOR_BACK_LEFT_CHANNEL
		pwm_channel_0.channel = MOTOR_BACK_LEFT_CHANNEL;
		pwm_channel_update_duty(PWM0, &pwm_channel_0, dutyCycleMotorBackLeft);

		// Set the PWM for MOTOR_FRONT_LEFT_CHANNEL
		pwm_channel_0.channel = MOTOR_FRONT_LEFT_CHANNEL;
		pwm_channel_update_duty(PWM0, &pwm_channel_0, dutyCycleMotorFrontLeft);

	}
}

// PIOB0, PIOB1, PIOB2, PIOB3 are LED's
int main(void)
{
    //PWM Setup							///////////////////////////////////////////////////
    /* Enable PWM peripheral clock */
    PMC->PMC_PCER0 |= PMC_PCER0_PID31;

    /* Disable PWM channels*/
    pwm_channel_disable(PWM0, MOTOR_FRONT_RIGHT_CHANNEL);
    pwm_channel_disable(PWM0, MOTOR_BACK_RIGHT_CHANNEL);
    pwm_channel_disable(PWM0, MOTOR_BACK_LEFT_CHANNEL);
    pwm_channel_disable(PWM0, MOTOR_FRONT_LEFT_CHANNEL);

    /* Set PWM clock A as PWM_FREQUENCY*MOTOR_PERIOD_VALUE (clock B is not used) */
    pwm_clock_t clock_setting = {
        .ul_clka = 150000000UL,
        .ul_clkb = 0,
        .ul_mck = 150000000UL
    };
    pwm_init(PWM0, &clock_setting);
    //pwm_init(PWM1, &clock_setting);

    /* Initialize PWM channel 0 */
    /* Period is left-aligned */
    pwm_channel_0.alignment = PWM_ALIGN_LEFT;
    /* Output waveform starts at a low level */
    pwm_channel_0.polarity = PWM_LOW;
    /* Use PWM clock A as source clock */
    pwm_channel_0.ul_prescaler = PWM_CMR_CPRE_CLKA;
    /* Period value of output waveform */
    pwm_channel_0.ul_period = MOTOR_PERIOD_VALUE;
    /* Duty cycle value of output waveform */
    pwm_channel_0.ul_duty = 0;

    /* Initialize PWM channel for MOTOR_FRONT_RIGHT */
    pwm_channel_0.channel = MOTOR_FRONT_RIGHT_CHANNEL;
    pwm_channel_init(PWM0, &pwm_channel_0);
    /* Enable channel counter event interrupt */
    pwm_channel_enable_interrupt(PWM0, MOTOR_FRONT_RIGHT_CHANNEL, 0);

    /* Initialize PWM channel for MOTOR_BACK_RIGHT */
    pwm_channel_0.channel = MOTOR_BACK_RIGHT_CHANNEL;
    pwm_channel_init(PWM0, &pwm_channel_0);
    /* Disable channel counter event interrupt */
    pwm_channel_disable_interrupt(PWM0, MOTOR_BACK_RIGHT_CHANNEL, 0);

    /* Initialize PWM channel for MOTOR_BACK_LEFT */
    pwm_channel_0.channel = MOTOR_BACK_LEFT_CHANNEL;
    pwm_channel_init(PWM0, &pwm_channel_0);
    /* Disable channel counter event interrupt */
    pwm_channel_disable_interrupt(PWM0, MOTOR_BACK_LEFT_CHANNEL, 0);

    /* Initialize PWM channel for MOTOR_FRONT_LEFT */
    pwm_channel_0.channel = MOTOR_FRONT_LEFT_CHANNEL;
    pwm_channel_init(PWM0, &pwm_channel_0);
    /* Disable channel counter event interrupt */
    pwm_channel_disable_interrupt(PWM0, MOTOR_FRONT_LEFT_CHANNEL, 0);


    /* Configure interrupt and enable PWM0 interrupt */
    NVIC_DisableIRQ(PWM0_IRQn);
    NVIC_ClearPendingIRQ(PWM0_IRQn);
    NVIC_SetPriority(PWM0_IRQn, 0);
    NVIC_EnableIRQ(PWM0_IRQn);

    /* Enable PWM channels */
    pwm_channel_enable(PWM0, MOTOR_FRONT_RIGHT_CHANNEL);
    pwm_channel_enable(PWM0, MOTOR_BACK_RIGHT_CHANNEL);
    pwm_channel_enable(PWM0, MOTOR_BACK_LEFT_CHANNEL);
    pwm_channel_enable(PWM0, MOTOR_FRONT_LEFT_CHANNEL);

    gpio_conf(PIOC, MODE_FRONT_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_BACK_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_BACK_LEFT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_FRONT_LEFT, PIO_OUTPUT);

    gpio_conf(PIOC, DIR_FRONT_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, DIR_BACK_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, DIR_BACK_LEFT, PIO_OUTPUT);
    gpio_conf(PIOC, DIR_FRONT_LEFT, PIO_OUTPUT);

    gpio_set_alt(PIOA, PIO_PA0, PIO_ALTA); // motor front right
    gpio_set_alt(PIOA, PIO_PA12, PIO_ALTB); // motor back right
    gpio_set_alt(PIOA, PIO_PA25, PIO_ALTB); // motor back left
    gpio_set_alt(PIOC, PIO_PC13, PIO_ALTB); // motor front left

    gpio_set(PIOC, MODE_FRONT_RIGHT);
    gpio_set(PIOC, MODE_BACK_RIGHT);
    gpio_set(PIOC, MODE_BACK_LEFT);
    gpio_set(PIOC, MODE_FRONT_LEFT);

    gpio_conf(PIOB, PIO_PB0, PIO_OUTPUT);

    gpio_set(PIOB, PIO_PB0);
    for (volatile uint32_t i = 0; i < 1000000; i++);
    gpio_clr(PIOB, PIO_PB0);
    for (volatile uint32_t i = 0; i < 1000000; i++);

    gpio_conf(PIOB, PIO_PB0, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB1, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB2, PIO_OUTPUT);
    gpio_conf(PIOB, PIO_PB3, PIO_OUTPUT);

    // uart_conf(UART0, 115200, UART_NO_PARITY);

    // configure SPI0
    spi_conf(SPI0, 1000000UL, SPI_MODE_0);

    nrf_conf();
    nrf_set_mode(NRF_MODE_RX);
    nrf_set_channel(0x1F);
    nrf_set_data_rate(NRF_DATARATE_1MBPS);
    nrf_set_power_output(NRF_RF_PWR_0);

    uint8_t addr[5] = {0, 1, 2, 3, 4};
    // just a 1 byte packet for simple test
    nrf_open_reading_pipe(NRF_PIPE_1, &addr[0], 5, 1);

    nrf_start_listening();

    uint8_t led_state[4] = {0};

    while (1)
    {
        while (!nrf_is_data_ready());

        uint8_t pos = 0;
        uint8_t pipe_num = nrf_receive_packet(&pos);
        nrf_rx_pipe_size(pipe_num);

        if (nrf_rx_buf[pos] == '1') led_state[0] = 10 - led_state[0];
        if (nrf_rx_buf[pos] == '2') led_state[1] = 10 - led_state[1];
        if (nrf_rx_buf[pos] == '3') led_state[2] = 10 - led_state[2];
        if (nrf_rx_buf[pos] == '4') led_state[3] = 10 - led_state[3];

        motorsMovementSpeed(led_state[0], led_state[1], led_state[2], led_state[3]);
    }

    return 0;
}





void motorsMovementSpeed(int frontRight, int backRight, int backLeft, int frontLeft)
{
	/**
		This function is for controlling the nominal speed of the main motors of the robots.

		@param All inputs can take a value between -100 to 100. positive number indicates CCW
			and negative number indicates Cw direction. The absolute value is the percentage
			of the speed or essentially the duty cycle of the PWM signal for that motor
		@return No output
	*/

	// Motors wont turn for any value out of range. The fault LED turns on
	if ((frontRight < -100) | (frontRight > 100) | (frontLeft < -100) | (frontLeft > 100) |
		(backRight < -100) | (backRight > 100) | (backLeft < -100) | (backRight > 100))
	{
		dutyCycleMotorFrontRight = 0;
		dutyCycleMotorBackRight = 0;
		dutyCycleMotorBackLeft = 0;
		dutyCycleMotorFrontLeft = 0;

		// Adding the LED fault turns on
	}

	//Front right motor signal
	if (frontRight > 0)
	{
		gpio_clr(PIOC, DIR_FRONT_RIGHT);
		dutyCycleMotorFrontRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontRight / 100);
	}
	else if (frontRight < 0)
	{
        gpio_set(PIOC, DIR_FRONT_RIGHT);
		frontRight = -frontRight;
		dutyCycleMotorFrontRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontRight / 100);
	}
	else
	{
		gpio_clr(PIOC, DIR_FRONT_RIGHT);
		dutyCycleMotorFrontRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontRight / 100);
	}

	//Back right motor signal
	if (backRight > 0)
	{
		gpio_clr(PIOC, DIR_BACK_RIGHT);
		dutyCycleMotorBackRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backRight / 100);
	}
	else if (backRight < 0)
	{
		gpio_set(PIOC, DIR_BACK_RIGHT);
		backRight = -backRight;
		dutyCycleMotorBackRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backRight / 100);
	}
	else
	{
		gpio_clr(PIOC, DIR_BACK_RIGHT);
		dutyCycleMotorBackRight = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backRight / 100);
	}

	//Back left motor signal
	if (backLeft > 0)
	{
		gpio_clr(PIOC, DIR_BACK_LEFT);
		dutyCycleMotorBackLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backLeft / 100);
	}
	else if (backLeft < 0)
	{
		gpio_set(PIOC, DIR_BACK_LEFT);
		backLeft = -backLeft;
		dutyCycleMotorBackLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backLeft / 100);
	}
	else
	{
		gpio_clr(PIOC, DIR_BACK_LEFT);
		dutyCycleMotorBackLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * backLeft / 100);
	}

	//Front left motor signal
	if (frontLeft > 0)
	{
		gpio_clr(PIOC, DIR_FRONT_LEFT);
		dutyCycleMotorFrontLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontLeft / 100);
	}
	else if (frontLeft < 0)
	{
		gpio_set(PIOC, DIR_FRONT_LEFT);
		frontLeft = -frontLeft;
		dutyCycleMotorFrontLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontLeft / 100);
	}
	else
	{
		gpio_clr(PIOC, DIR_FRONT_LEFT);
		dutyCycleMotorFrontLeft = MOTOR_PERIOD_VALUE - (MOTOR_PERIOD_VALUE * frontLeft / 100);
	}

	return;
}
