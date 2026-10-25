#include "motor_mapping.h"
#include <stdio.h>

#define ACC_MAX 255
#define MOTOR_MAX 100
#define DEADZONE 25

static int clamp_int(int val, int min, int max)
{
    if (val < min)
        return min;
    
    if (val > max)
        return max;
    
    return val;
}

static int apply_deadzone(int val)
{
    if (val > -DEADZONE && val < DEADZONE)
        return 0;
    
    return val;
}

static int acc_to_motor(int acc_val)
{
    acc_val = clamp_int(acc_val, -ACC_MAX, ACC_MAX);
    acc_val = apply_deadzone(acc_val);

    return (acc_val * MOTOR_MAX / ACC_MAX);
}

motor_command_t motor_mapping_from_acc(int x, int y)
{
    int throttle = acc_to_motor(y);
    int steering = acc_to_motor(x);

    motor_command_t cmd;

    cmd.left = throttle + steering;
    cmd.right = throttle - steering;

    cmd.left = clamp_int(cmd.left, -MOTOR_MAX, MOTOR_MAX);
    cmd.right = clamp_int(cmd.right, -MOTOR_MAX, MOTOR_MAX);

    return cmd;
}