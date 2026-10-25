/* File:   pwm_test2.c
   Date:   15 April 2013
   Descr:  This example starts two channels simultaneously; one inverted
           with respect to the other.
*/
#include "pwm.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"
#include "pwm_driver.h"



// If you are using PWM to drive a motor you will need
// to choose a lower frequency!

static const pwm_cfg_t pwm_AIN1_cfg =
{
    .pio = MOTOR_L1_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm_BIN1_cfg =
{


    .pio = MOTOR_R1_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm_AIN2_cfg =
{
    .pio = MOTOR_L2_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static const pwm_cfg_t pwm_BIN2_cfg =
{
    .pio = MOTOR_R2_PWM_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static Motor motor_L;
static Motor motor_R;

bool motors_init(void)
{
    motor_L = (Motor){pwm_init(&pwm_AIN1_cfg), pwm_init(&pwm_AIN2_cfg)};
    motor_R = (Motor){pwm_init(&pwm_BIN1_cfg), pwm_init(&pwm_BIN2_cfg)};

    pwm_channels_start (pwm_channel_mask (motor_L.pwm_1) | 
                        pwm_channel_mask (motor_L.pwm_2) | 
                        pwm_channel_mask (motor_R.pwm_1) | 
                        pwm_channel_mask (motor_R.pwm_2));
    
    return motor_L.pwm_1 != NULL && motor_L.pwm_2 != NULL && motor_R.pwm_1 != NULL && motor_R.pwm_2 != NULL;
}

bool drive_motor(Wheel_side side, int speed) {

    Motor selected_motor;
    if (speed < -100) {
        speed = -100;
    }
    if (speed > 100) {
        speed = 100;
    }
    
    switch (side) {
        case LEFT:
            //printf("Selecting Left\n");
            selected_motor = motor_L;
            break;
        case RIGHT:
            //printf("Selecting Right\n");
            selected_motor = motor_R;
            break;
        default:
            return false;
    }

    if (speed < 0) {
        pwm_period_t duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, -speed); 
        pwm_duty_set(selected_motor.pwm_1, PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0));
        pwm_duty_set(selected_motor.pwm_2, duty);

    } else {
        pwm_period_t duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, speed); 
        pwm_duty_set(selected_motor.pwm_2, PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 0));
        pwm_duty_set(selected_motor.pwm_1, duty);
    }
    

    return true;
}
