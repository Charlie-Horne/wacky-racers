/* File:   sleep_mode.c
   Author: Dominic McNulty & Cade Hinman
   Date:   1 May 2026
   Descr:  SAM4S Backup sleep mode using PA5/WKUP4 wake-up button.
*/

#include <stdio.h>
#include <stdint.h>

#include "sleep_mode.h"
#include "pio.h"
#include "delay.h"


#define REG32(addr) (*(volatile uint32_t *)(addr))
#define UDP_TXVC       REG32(0x40034074u)

/* SAM4S Supply Controller registers */
#define SUPC_CR     REG32(0x400E1410u)
#define SUPC_WUMR   REG32(0x400E141Cu)
#define SUPC_WUIR   REG32(0x400E1420u)
#define SUPC_SR     REG32(0x400E1424u)

/* SUPC_CR fields */
#define SUPC_KEY            (0xA5u << 24)

/* Wake-up input setup */
#define WKUP_ENABLE(wkup_num)      (1u << (wkup_num))
#define WKUP_ACTIVE_HIGH(wkup_num) (1u << ((wkup_num) + 16))

/*
 * WKUPDBC debounce setting in SUPC_WUMR.
 *
 * 3 means 512 slow-clock cycles.
 * With a 32 kHz slow clock, this is about 16 ms.
 */
#define SUPC_WUMR_WKUPDBC_512_SLCK (3u << 12)


void
sleep_mode_init (pio_t wake_button_pio)
{
    /*
     * Button is connected to ground when pressed:
     *
     * unpressed = high
     * pressed   = low
     *
     * Therefore use internal pull-up.
     */
    pio_config_set(wake_button_pio, PIO_PULLUP);

    /*
     * Clear old wake-up flags.
     * SUPC_SR wake status flags are cleared on read.
     */
    (void) SUPC_SR;
}

static void
sleep_park_unused_pins (void)
{
    /* Stop the radio from being back-powered through SAM4S output pins. */
    pio_config_set(RADIO_CE_PIO,    PIO_OUTPUT_LOW);
    pio_config_set(RADIO_CS_PIO, PIO_OUTPUT_LOW);
    pio_config_set(RADIO_MOSI_PIO,  PIO_OUTPUT_LOW);
    pio_config_set(RADIO_SPCK_PIO,  PIO_OUTPUT_LOW);

    pio_config_set(RADIO_MISO_PIO, PIO_PULLDOWN);
    pio_config_set(RADIO_IRQ_PIO,  PIO_PULLDOWN);

    pio_config_set(LEDTAPE_PIO, PIO_OUTPUT_LOW);
    pio_config_set(IDLE_EN_PIO, PIO_OUTPUT_LOW);

    /* Disable USB transceiver and disconnect internal D+ pullup. */
    UDP_TXVC = UDP_TXVC_TXVDIS;

    delay_ms(10);
}


void
sleep_mode_enter_backup (pio_t wake_button_pio)
{

    sleep_park_unused_pins();

    printf("Entering Backup sleep mode\n");
    fflush(stdout);

    /*
     * pio_input_get(...) == 0 means pressed.
     */
    while (!pio_input_get(wake_button_pio)) {
        delay_ms(10);
    }

    delay_ms(50);    // release debounce

    // Clear previous wake-up status.
    (void) SUPC_SR;

    /*
     * Configure PA5/WKUP4 as the wake source.
     */
    SUPC_WUIR = WKUP_ENABLE(SLEEP_MODE_WAKEUP_NUM);

    // Enable wake-up debounce.
    SUPC_WUMR = SUPC_WUMR_WKUPDBC_512_SLCK;

    /*
     * Enter Backup mode.
     * Turns of internal regulator/core supply.
     * SAM4S will reset when PA5 button pressed again
     */
    SUPC_CR = SUPC_KEY | SUPC_CR_VROFF;

    while (1) {
        /* Should never reach here. */
    }
}