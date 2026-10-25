#ifndef BUZZER_TEST_H
#define BUZZER_TEST_H

#include "mat91lib.h"

#define PWM_FREQ_HZ 2500

bool buzzer_init(void);
bool buzzer_set(int duty_percent);

#endif /* BUZZER_TEST_H */