#include "pwm.h"

#define PWM_ALIGN_LEFT     (0 << 8)
#define PWM_ALIGN_CENTER   (1 << 8)

#define PWM_POLARITY_LOW   (0 << 9)
#define PWM_POLARITY_HIGH  (1 << 9)

// pwm period to be used when calculating the duty cycle
// this value is initialized when the pwm starts up and remains constant
static int pwm_period[4] = {0};

static uint32_t pwm_clocks_generate(uint32_t freq)
{
	uint32_t divisors[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
	uint32_t pre = 0;
	uint32_t div;
    // find prescaler and divisor values that closely match our frequency input
    // return a value that is directly pluggable into the register
	do {
		div = (F_PCLK / divisors[pre]) / freq;
		if (div <= 256) break;
		pre++;
	} while (pre < 11);

	if (pre < 11) return (div | (pre << 8));
	else return 0;
}

void pwm_conf(Pwm *pwm, uint32_t freq)
{
	// enable the peripheral clock
    if (pwm == PWM0) PMC->PMC_PCER0 |= PMC_PCER0_PID31;
    else if (pwm == PWM1) PMC->PMC_PCER1 |= PMC_PCER1_PID60;
    else return;      // unsupported pwm channel
    // disable channels before reconfiguring
    pwm->PWM_DIS = 0b1111;
    // set up the prescaler and divider according to our frequency parameter
    uint32_t reg_val = pwm_clocks_generate(freq);
    // if we returned an error generating a suitable clock, the pwm can't be set up
    if (!reg_val) return;
    // set the clock register
    pwm->PWM_CLK = reg_val;
}

void pwm_channel_conf(Pwm *pwm, uint8_t ch, uint32_t period)
{
	// update the period variable so that it can be used to calculate duty cycle
	pwm_period[ch] = period;
    // use PWM clock A as source clock, left aligned, start waveform at low
    pwm->PWM_CH_NUM[ch].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_ALIGN_LEFT | PWM_POLARITY_LOW;
    // initial duty cycle should be 0
	pwm->PWM_CH_NUM[ch].PWM_CDTY = 100;
    // set the period register
	pwm->PWM_CH_NUM[ch].PWM_CPRD = period;
    // don't care about setting the dead time
    // don't enable output override
    pwm->PWM_OS = 0;
    // make this channel a synchronous channel
    pwm->PWM_SCM |= (1 << ch);
    // force pwm output to low when fault occurs
    // change this for both pwmh and pwml outputs
    pwm->PWM_FPV1 &= (~(1 << ch) | ~(1 << (ch + 16)));
    // don't go to high Z on fault
    pwm->PWM_FPV2 &= (~(1 << ch) | ~(1 << (ch + 16)));
    // disable all fault protections
    pwm->PWM_FPE &= ~(0xFF << (ch * 8));
    // disable push/pull mode
    pwm->PWM_CH_NUM[ch].PWM_CMR &= (~PWM_CMR_PPM);
	// enable the pwm channel
	pwm->PWM_ENA = (1 << ch);
}

void pwm_channel_disable(Pwm *pwm, uint8_t ch)
{
    // disable this pwm channel
	pwm->PWM_DIS = (1 << ch);
}

void pwm_channel_set_duty(Pwm *pwm, uint8_t ch, uint8_t duty)
{
	float duty_percent = duty / 100.0;
	// set the duty cycle according to the period
	uint32_t reg_val = (uint32_t)(pwm_period[ch] - (float)(pwm_period[ch] * duty_percent));
	pwm->PWM_CH_NUM[ch].PWM_CDTY = reg_val;
}
