#include "pwm.h"
#include "pio.h"
#include "buzzer_test.h"

// static const pwm_cfg_t pwm_AIN1_cfg =
// {
// .pio = BUZZER_PWM_PIO,
// .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
// .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, 50),
// .align = PWM_ALIGN_LEFT,
// .polarity = PWM_POLARITY_HIGH,
// .stop_state = PIO_OUTPUT_LOW
// };

// static pwm_t buzzer_pwm;

// bool buzzer_init(void)
// {
// buzzer_pwm = pwm_init(&pwm_AIN1_cfg);
// pwm_channels_start(pwm_channel_mask(buzzer_pwm));
// return buzzer_pwm != NULL;
// }

// bool buzzer_set(int duty_percent)
// {
// if (duty_percent < 0)   duty_percent = 0;
// if (duty_percent > 100) duty_percent = 100;

// pwm_duty_set(buzzer_pwm, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_percent));
// return true;
// }

