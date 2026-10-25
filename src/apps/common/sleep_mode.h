/* File:   sleep_mode.h
   Author: Dominic McNulty & Cade Hinman
   Date:   1 May 2026
   Descr:  SAM4S Backup sleep mode using PA5/WKUP4 wake-up button.
*/

#ifndef SLEEP_MODE_H
#define SLEEP_MODE_H

#include <stdint.h>
#include "pio.h"

// PA5 maps to WKUP4.
#define SLEEP_MODE_WAKEUP_NUM 4

void sleep_mode_init (pio_t wake_button_pio);
void sleep_mode_enter_backup (pio_t wake_button_pio);

#endif