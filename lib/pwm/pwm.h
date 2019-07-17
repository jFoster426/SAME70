#ifndef PWM_H
#define PWM_H

#include "same70q21.h"
#include "../uart/uart.h"

// uint32_t pwm_clocks_generate(uint32_t freq);

void pwm_conf(Pwm *pwm, uint32_t freq);

void pwm_channel_conf(Pwm *pwm, uint8_t ch, uint32_t period);

void pwm_channel_disable(Pwm *pwm, uint8_t ch);

void pwm_channel_set_duty(Pwm *pwm, uint8_t ch, uint8_t duty);

#endif
