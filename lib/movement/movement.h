#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdarg.h>
#include <stdio.h>

#include "../gpio/gpio.h"
#include "../pwm/pwm.h"

#define MOTOR_PWM              PWM0
#define MOTOR_PERIOD_VALUE     8500 // this value works for the motors that we have
#define MOTOR_DIR_FORWARD      1
#define MOTOR_DIR_REVERSE      0

typedef struct {
    Pio* const dir_port;         // direction pio port
    const uint32_t dir_pin;      // pio direction pio pin
    Pio* const pwm_port;         // pwm pio port
    const uint32_t pwm_pin;      // pwm pio pin
    const uint8_t pwm_pio_mode;  // pwm pio pin mode
    const uint8_t pwm_channel;   // pwm channel used for this motor
    uint8_t dir;           // direction of the current motor
    uint8_t duty;          // duty cycle of the current motor
} motor_t;

motor_t motor_FL = {PIOC, PIO_PC22, PIOC, PIO_PC13, PIO_ALTA, 0, 0, 0};
motor_t motor_FR = {PIOC, PIO_PC2,  PIOA, PIO_PA0,  PIO_ALTB, 1, 0, 0};
motor_t motor_BL = {PIOC, PIO_PC16, PIOA, PIO_PA15, PIO_ALTB, 2, 0, 0};
motor_t motor_BR = {PIOC, PIO_PC28, PIOA, PIO_PA12, PIO_ALTB, 3, 0, 0};

void motors_conf(int num_motors, ...)
{
    pwm_conf(PWM0, F_PCLK);

    va_list ap;

    va_start(ap, num_motors);

    for (uint8_t i = 0; i < num_motors; i++)
    {
        motor_t m = va_arg(ap, motor_t);
        pwm_channel_conf(MOTOR_PWM, m.pwm_channel, MOTOR_PERIOD_VALUE);
        gpio_conf(m.dir_port, m.dir_pin, PIO_OUTPUT);
        gpio_conf(m.pwm_port, m.pwm_pin, m.pwm_pio_mode);
    }

    va_end(ap);

    // **** won't need this when new drivers come (mode=1)
#define MODE_FRONT_RIGHT    PIO_PC3
#define MODE_BACK_RIGHT     PIO_PC9
#define MODE_BACK_LEFT      PIO_PC17
#define MODE_FRONT_LEFT     PIO_PC23

    gpio_conf(PIOC, MODE_FRONT_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_BACK_RIGHT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_BACK_LEFT, PIO_OUTPUT);
    gpio_conf(PIOC, MODE_FRONT_LEFT, PIO_OUTPUT);

    gpio_set(PIOC, MODE_FRONT_RIGHT);
    gpio_set(PIOC, MODE_BACK_RIGHT);
    gpio_set(PIOC, MODE_BACK_LEFT);
    gpio_set(PIOC, MODE_FRONT_LEFT);
    // ****
}

void motor_write_individual(motor_t motor, uint8_t speed, uint8_t dir)
{
    if (speed > 100) speed = 100; // unsigned int can't go below 0
    motor.duty = speed;
    motor.dir = dir;
    gpio_write(motor.dir_port, motor.dir_pin, dir);
    pwm_channel_set_duty(PWM0, motor.pwm_channel, 5);
}

#endif
