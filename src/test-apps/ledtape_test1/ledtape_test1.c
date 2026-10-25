/* File:   ledtape_test11.c
   Date:   30 Jan 2020
   Descr:  Test ledtape
*/

#include <pio.h>
#include "target.h"
#include "pacer.h"
#include "ledtape.h"



/*
    This test app shows how to program the WS2318B LED tape.
    In this example the ledtape_write() function is used to send the appropriate
    GRB color information down the LED tape. It takes care of all the tricky
    timings, you just need to feed it an array of color values.

    ledtape_write(
        pin - This is the pin you want to send the data on. Should be connected
              to your level shifter.
        data - This is the array of GRB data (8 bit values, 24 bits per pixel).
        size - This is the size of the array in bytes.
    )

    You will also see use of the pacer module. This isn't really needed, but
    makes it easier to see the data packets being sent if you put the data
    signal on an oscilloscope.

    Suggestions:
    * Change the number of LEDs to match yours (or move the NUM_LEDS definition
      to your target file)
    * Change the color being set on all the LEDs.
    * Make an interesting pattern.
    * Make the pattern scroll down the LEDs (see ledtape_test2 for an option).

    Troubleshooting: check the frequency of the generated waveform; if it is not
    close to 800 kHz, adjust the value of LEDTAPE_TPERIOD by defining it in
    target.h. (See src/mmculib/ledtape/ledtape.h for more info.)
*/



#include <pio.h>
#include <stdint.h>
#include "target.h"
#include "ledtape.h"
#include "ledbuffer.h"

#define NUM_LEDS 22

static void
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

int
main(void)
{
    ledbuffer_t *leds = ledbuffer_init(LEDTAPE_PIO, NUM_LEDS);

    rainbow_startup(leds);

    while (1)
    {
        /* rest of your program */
    }
}
