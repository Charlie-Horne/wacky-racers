#include "accelerometer.h"

#include "config.h"
#include "panic.h"

#ifndef ADXL345_ADDRESS
#error ADXL345_ADDRESS must be defined
#endif

static twi_cfg_t adxl345_twi_cfg =
{
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR(100000),   /* 100 kHz I2C */
    .slave_addr = 0
};

adxl345_t *accelerometer_init(void)
{
    twi_t adxl345_twi;
    adxl345_t *adxl345;

    adxl345_twi = twi_init(&adxl345_twi_cfg);

    if (!adxl345_twi)
        panic(LED_ERROR_PIO, 1);

    adxl345 = adxl345_init(adxl345_twi, ADXL345_ADDRESS);

    if (!adxl345)
        panic(LED_ERROR_PIO, 2);

    return adxl345;
}

int accelerometer_is_ready(adxl345_t *accel)
{
    return adxl345_is_ready(accel);
}

int accelerometer_read(adxl345_t *accel, accel_data_t *data)
{
    int16_t raw_accel[3];

    if (!adxl345_accel_read(accel, raw_accel))
        return 0;

    data->x = raw_accel[0];
    data->y = raw_accel[1];
    data->z = raw_accel[2];

    return 1;
}