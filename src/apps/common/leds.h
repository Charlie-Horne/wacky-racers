/* File:   leds.c
   Author: Dominic McNulty & Cade Hinman
   Date:   11 May 2026
   Descr:  Drives LED pins for different colors
*/

#ifndef LEDS_H
#define LEDS_H

#define NUM_LEDS 22

#include "ledbuffer.h"

void rainbow_startup(ledbuffer_t *leds);
void leds_off(ledbuffer_t *leds);


#endif