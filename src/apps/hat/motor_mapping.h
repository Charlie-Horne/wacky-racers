#ifndef MOTOR_MAPPING_H
#define MOTOR_MAPPING_H

#include <stdint.h>

typedef struct
{
    int left;
    int right;
} motor_command_t;

motor_command_t motor_mapping_from_acc(int x, int y);

#endif /* MOTOR_MAPPING_H */