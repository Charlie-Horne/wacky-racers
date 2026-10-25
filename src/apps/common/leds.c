#include <pio.h>
#include <stdint.h>
#include "target.h"
#include "ledtape.h"
#include "ledbuffer.h"
#include "delay.h"
#include "leds.h"

void
rainbow_startup(ledbuffer_t *leds)
{
    ledbuffer_clear(leds);

    ledbuffer_set(leds, 0,  255, 0,   0);    // red
    ledbuffer_set(leds, 1,  255, 70,  0);
    ledbuffer_set(leds, 2,  255, 130, 0);
    ledbuffer_set(leds, 3,  255, 190, 0);
    ledbuffer_set(leds, 4,  255, 255, 0);    // yellow
    ledbuffer_set(leds, 5,  190, 255, 0);
    ledbuffer_set(leds, 6,  120, 255, 0);
    ledbuffer_set(leds, 7,  60,  255, 0);
    ledbuffer_set(leds, 8,  0,   255, 0);    // green
    ledbuffer_set(leds, 9,  0,   255, 70);
    ledbuffer_set(leds, 10, 0,   255, 140);
    ledbuffer_set(leds, 11, 0,   255, 210);
    ledbuffer_set(leds, 12, 0,   255, 255);  // cyan
    ledbuffer_set(leds, 13, 0,   190, 255);
    ledbuffer_set(leds, 14, 0,   130, 255);
    ledbuffer_set(leds, 15, 0,   70,  255);
    ledbuffer_set(leds, 16, 0,   0,   255);  // blue
    ledbuffer_set(leds, 17, 70,  0,   255);
    ledbuffer_set(leds, 18, 130, 0,   255);
    ledbuffer_set(leds, 19, 190, 0,   255);
    ledbuffer_set(leds, 20, 255, 0,   255);  // magenta
    ledbuffer_set(leds, 21, 255, 0,   130);

    ledbuffer_write(leds);
}

void
leds_off(ledbuffer_t *leds)
{
    ledbuffer_clear(leds);   // Sets all LEDs in the buffer to RGB 0,0,0
    ledbuffer_write(leds);   // Sends the off frame to the LED tape
}