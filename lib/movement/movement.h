#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../gpio/gpio.h"
#include "../pwm/pwm.h"
#include "../math/math.h"

#define MOTOR_PWM              PWM0
#define MOTOR_PERIOD_VALUE     8500 // this value works for the motors that we have
#define MOTOR_DIR_FORWARD      1
#define MOTOR_DIR_REVERSE      -1

#define MOTOR_FRONT_LEFT       1
#define MOTOR_FRONT_RIGHT      2
#define MOTOR_BACK_LEFT        3
#define MOTOR_BACK_RIGHT       4

typedef struct {
    const uint8_t pos;           // this motor's position on the robot
    Pio* const dir_port;         // direction pio port
    const uint32_t dir_pin;      // pio direction pio pin
    Pio* const pwm_port;         // pwm pio port
    const uint32_t pwm_pin;      // pwm pio pin
    const uint8_t pwm_pio_mode;  // pwm pio pin mode
    const uint8_t pwm_channel;   // pwm channel used for this motor
    uint8_t dir;                 // direction of the current motor
    uint8_t duty;                // duty cycle of the current motor
} motor_t;

void motors_conf(motor_t* motors[])
{
    pwm_conf(PWM0, F_PCLK);

    // todo: change 4 to dynamic size of the motor array
    for (uint8_t i = 0; i < 4; i++)
    {
        pwm_channel_conf(MOTOR_PWM, motors[i]->pwm_channel, MOTOR_PERIOD_VALUE);
        gpio_conf(motors[i]->dir_port, motors[i]->dir_pin, PIO_OUTPUT);
        gpio_conf(motors[i]->pwm_port, motors[i]->pwm_pin, motors[i]->pwm_pio_mode);
    }

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
    gpio_write(motor.dir_port, motor.dir_pin, (dir == MOTOR_DIR_FORWARD) ? 1 : 0); // since motor_dir_reverse is actually -1
    pwm_channel_set_duty(PWM0, motor.pwm_channel, 5);
}

void motors_write_direction(motor_t* motors[], int pwr, int theta, int phi)
{
    // phi needs to be implemented properly, just set to 0 right now
    uint8_t duty;

    for (uint8_t i = 0; i < 4; i++)
    {
        if (motors[i]->pos == MOTOR_FRONT_LEFT) duty = pwr * fast_sin(theta - 300) - phi;
        else
        if (motors[i]->pos == MOTOR_FRONT_RIGHT) duty = pwr * fast_sin(theta - 60) + phi;
        else
        if (motors[i]->pos == MOTOR_BACK_LEFT) duty = pwr * fast_sin(theta - 230) - phi;
        else
        if (motors[i]->pos == MOTOR_BACK_LEFT) duty = pwr * fast_sin(theta - 130) + phi;
        else break;  // not a valid motor location
        motor_write_individual(*motors[i], duty, sign(duty));
    }
}

#endif
