/* File:   communication.c
   Author: Dominic McNulty & Cade Hinman
   Date:   11 May 2026
   Descr:  Radio communication used by both Wacky Hat and Wacky Racer.
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "config.h"
#include "nrf24.h"
#include "spi.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"
#include "communication.h"

#define RADIO_CHANNEL_COUNT 4

static const uint8_t radio_channels[RADIO_CHANNEL_COUNT] =
{
    12,
    38,
    76,
    104
};

static const pio_t channel_select_pios[RADIO_CHANNEL_COUNT] =
{
    CHANNEL_SELECT_0,
    CHANNEL_SELECT_1,
    CHANNEL_SELECT_2,
    CHANNEL_SELECT_3
};

static nrf24_cfg_t nrf24_cfg =
{
    .channel = RADIO_DEFAULT_CHANNEL,
    .address = RADIO_ADDRESS,
    .payload_size = RADIO_PAYLOAD_SIZE,
    .ce_pio = RADIO_CE_PIO,
    .irq_pio = RADIO_IRQ_PIO,
    .spi =
    {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0,
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8
    },
};

static nrf24_t *nrf;

static uint8_t radio_select_channel_from_jumpers(void)
{
    uint8_t i;

    for (i = 0; i < RADIO_CHANNEL_COUNT; i++)
    {
        pio_config_set(channel_select_pios[i], PIO_INPUT);
    }

    delay_ms(1);

    for (i = 0; i < RADIO_CHANNEL_COUNT; i++)
    {
        /* Jumper to GND means selected, so input reads low. */
        if (!pio_input_get(channel_select_pios[i]))
        {
            return radio_channels[i];
        }
    }

    return RADIO_DEFAULT_CHANNEL;
}

void initialize_radio(void)
{
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    delay_ms(100);

    nrf24_cfg.channel = radio_select_channel_from_jumpers();

    nrf = nrf24_init(&nrf24_cfg);

    while (!nrf)
    {
        nrf = nrf24_init(&nrf24_cfg);
        printf("ERROR: nrf24_init failed\n");
        panic(LED_STATUS_PIO, 2);
    }

    nrf24_listen(nrf);
}

int serial_read_line(char *buffer, uint8_t buffer_size)
{
    static uint8_t index = 0;
    int ch = getchar();

    if (ch == EOF || buffer_size == 0)
        return 0;

    if (ch == '\r' || ch == '\n')
    {
        if (index == 0)
            return 0;

        buffer[index] = '\0';
        index = 0;
        return 1;
    }

    if (index < buffer_size - 1)
    {
        buffer[index++] = ch;
    }
    else
    {
        index = 0;
        printf("ERROR: serial input too long\n");
    }

    return 0;
}

uint8_t radio_read_command(void *rx_buffer, uint8_t radio_size)
{
    if (!nrf || !rx_buffer || radio_size == 0)
        return 0;

    return nrf24_read(nrf, rx_buffer, radio_size);
}

int radio_send_command(const char *message)
{
    char tx_buffer[RADIO_PAYLOAD_SIZE];

    if (!nrf || !message)
        return 0;

    memset(tx_buffer, 0, sizeof(tx_buffer));
    snprintf(tx_buffer, sizeof(tx_buffer), "%s", message);

    if (!nrf24_write(nrf, tx_buffer, RADIO_PAYLOAD_SIZE))
    {
        nrf24_listen(nrf);
        return 0;
    }

    nrf24_listen(nrf);

    return 1;
}

void communication_print_config(void)
{
    printf("Radio channel: %u\n", nrf24_cfg.channel);
    printf("Radio address: 0x%012llx\n", RADIO_ADDRESS);
    printf("Radio payload size: %u\n", RADIO_PAYLOAD_SIZE);
}