#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include "adxl345.h"

typedef struct
{
    int x;
    int y;
    int z;
} accel_data_t;

adxl345_t *accelerometer_init(void);

int accelerometer_is_ready(adxl345_t *accel);

int accelerometer_read(adxl345_t *accel, accel_data_t *data);

#endif