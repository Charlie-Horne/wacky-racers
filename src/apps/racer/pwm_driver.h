#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "mat91lib.h"

#define PWM_FREQ_HZ 10e3

typedef enum {
    LEFT,
    RIGHT
} Wheel_side;

typedef struct {
    pwm_t pwm_1;
    pwm_t pwm_2;
} Motor;

bool 
motors_init(void);

bool 
drive_motor(Wheel_side side, int speed);

#endif /* PWM_DRIVER_H */




