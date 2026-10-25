/* File:   radio_test_2
/* File:   radio_test_2
   Descr:  Bidirectional nRF24 USB serial radio chat.
           Type into USB serial on either board to transmit.
           Received packets are printed to USB serial.
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "nrf24.h"
#include "spi.h"
#include "usb_serial.h"
#include "pio.h"
#include "pacer.h"
#include "delay.h"
#include "panic.h"
#include "motor_mapping.h"

#define RADIO_CHANNEL 4
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 32

#define RADIO_POWER_ENABLE_PIO IDLE_EN_PIO

#define PACER_RATE 1000

int main(void)
{
    spi_cfg_t spi_cfg =
    {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0,
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8
    };

    nrf24_cfg_t nrf24_cfg =
    {
        .channel = RADIO_CHANNEL,
        .address = RADIO_ADDRESS,
        .payload_size = RADIO_PAYLOAD_SIZE,
        .ce_pio = RADIO_CE_PIO,
        .irq_pio = RADIO_IRQ_PIO,
        .spi = spi_cfg,
    };

    nrf24_t *nrf;

    char tx_buffer[RADIO_PAYLOAD_SIZE];
    char rx_buffer[RADIO_PAYLOAD_SIZE + 1];

    char serial_buffer[128];
    uint8_t serial_index = 0;

    /* Configure LEDs. */
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_LOW);
    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);

    /* Redirect printf(), getchar(), etc. to USB serial. */
    usb_serial_stdio_init();

    pacer_init(PACER_RATE);

#ifdef RADIO_POWER_ENABLE_PIO
    /* Enable radio regulator if present. */
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);

    /*
       This delay matters. Without it, the radio regulator may not have
       settled before nrf24_init() runs.
    */
    delay_ms(100);
#endif

    printf("\nRadio chat test starting\n");
    printf("Channel: %d\n", RADIO_CHANNEL);
    printf("Address: 0x%012llx\n", RADIO_ADDRESS);
    printf("Payload size: %d\n", RADIO_PAYLOAD_SIZE);

    /* Initialise nRF24 radio. */
    nrf = nrf24_init(&nrf24_cfg);

    if (!nrf)
    {
        printf("ERROR: nrf24_init failed\n");
        panic(LED_ERROR_PIO, 2);
    }

    printf("Radio initialised\n");

    /* Default state should be receive mode. */
    nrf24_listen(nrf);

    printf("Listening. Type a message and press enter to transmit.\n\n");

    while (1)
    {
        uint8_t bytes;

        pacer_wait();

        /*
           1. Check radio receive.
           This keeps the board listening most of the time.
        */
        bytes = nrf24_read(nrf, rx_buffer, RADIO_PAYLOAD_SIZE);
        if (bytes != 0)
        {
            rx_buffer[bytes] = '\0';

            printf("RX: %s\n", rx_buffer);

            pio_output_toggle(LED_STATUS_PIO);
        }

        /*
           2. Check USB serial input.
           This builds a line character-by-character.
           
           This assumes getchar() is non-blocking in your usb_serial stdio
           implementation and returns EOF when no character is available.
        */

        int L = 0;
        int R = 0;
        motor_mapping_from_acc(L, R)
        int ch = getchar();

        if (ch != EOF)
        {
            if (ch == '\r' || ch == '\n')
            {
                if (serial_index > 0)
                {
                    serial_buffer[serial_index] = '\0';
                    printf("TX: %s\n", serial_buffer);

                    if (!radio_send_string(nrf, serial_buffer))
                    {
                        printf("ERROR: TX failed\n");
                        pio_output_set(LED_ERROR_PIO, 0);
                    }
                    else
                    {
                        pio_output_set(LED_ERROR_PIO, 1);
                    }
                }
            }
            else
            {
                if (serial_index < sizeof(serial_buffer) - 1)
                {
                    serial_buffer[serial_index++] = ch;
                }
                else
                {
                    /*
                       Serial line too long. Reset rather than overflowing.
                    */
                    serial_index = 0;
                    printf("ERROR: serial input too long\n");
                }
            }
        }
    }

    return 0;
}