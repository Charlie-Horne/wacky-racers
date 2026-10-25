/* File:   racer.c
   Author: Dominic McNulty & Cade Hinman
   Date:   13th May 2026
   Descr:  Full functioning racer code for Wacky Racers
*/

// Main Includes
#include <stdio.h>
#include <string.h>
#include "mcu.h"
#include "pio.h"
#include "target.h"
#include "delay.h"
#include "adc.h"

// Peripherals
#include "pwm.h"

// Drivers
#include "panic.h"
#include "usb_serial.h"
#include "pacer.h"
#include "led.h"
#include "button.h"
#include "nrf24.h"
#include "spi.h"

// Created
#include "communication.h"
#include "sleep_mode.h"
#include "pwm_driver.h"
#include "leds.h"

/* Main scheduler tick rate. */
#define PACER_RATE 500

/* Scheduled task rates in Hz. */
#define LED_FLASH_RATE 2
#define BUTTON_POLL_RATE 20
#define ADC_POLL_RATE 1
#define BUMPER_TX_RATE 20

#define LED_TICKS       (PACER_RATE / (LED_FLASH_RATE * 2))
#define BUTTON_TICKS    (PACER_RATE / BUTTON_POLL_RATE)
#define ADC_TICKS       (PACER_RATE / ADC_POLL_RATE)
#define BUMPER_TX_TICKS (PACER_RATE / BUMPER_TX_RATE)

/* Define LED configuration.  */
static const led_cfg_t led1_cfg =
{
    .pio = LED_STATUS_PIO,
    .active = 1
};

static const led_cfg_t led2_cfg =
{
    .pio = LED_ERROR_PIO,
    .active = 1
};

/* Define button configuration.  */
static const button_cfg_t button_sleep_cfg =
{
    .pio = BUTTON_SLEEP_PIO
};

static const adc_cfg_t adc_cfg =
{
    .bits = 12,
    .channels = BIT(LED_POW_CHANNEL),
    .trigger = ADC_TRIGGER_SW,
    .clock_speed_kHz = 1000
};

// Low power ADC input
static adc_t LED_POW;
static uint16_t adc_raw;
static uint16_t adc_count = 0;

// Sleep Button
static button_t button_sleep;

//LEDs
static led_t led1;
static led_t led2;
static ledbuffer_t *leds;

int initialize_peripherals(void) {

    mcu_jtag_disable();

    // Redirect stdio to USB serial
    if (usb_serial_stdio_init () < 0)
    panic(LED_STATUS_PIO, 4);

    LED_POW = adc_init(&adc_cfg);

    // Bumpers
    pio_config_set(BUMPER_PIO, PIO_INPUT);

    // Motors
    motors_init();

    // Button
    button_sleep = button_init (&button_sleep_cfg);
    button_poll_count_set (BUTTON_POLL_COUNT (PACER_RATE));

    // LEDs
    led1 = led_init (&led1_cfg);
    led_set (led1, LED_INACTIVE);

    led2 = led_init (&led2_cfg);
    led_set (led2, LED_INACTIVE);

    leds = ledbuffer_init(LEDTAPE_PIO, NUM_LEDS);
    rainbow_startup(leds);

    // IDLE EN PIO PIN for H-bridge and Radio Regulator
    pio_config_set (IDLE_EN_PIO, PIO_OUTPUT_HIGH);

    // Wait long enough before initalizing radio otherwise error
    delay_ms(100);

    initialize_radio();

    delay_ms(5000);

    communication_print_config();

    pacer_init (PACER_RATE);

    return 1;
}

int
main (void)
{
    initialize_peripherals();

    uint32_t led_ticks = 0;
    uint32_t button_ticks = 0;
    uint32_t bumper_ticks = 0;
    
    char rx_buffer[RADIO_PAYLOAD_SIZE + 1];

    // // Debounces button after waking from sleep mode
    // while (!pio_input_get(BUTTON_SLEEP_PIO)) {
    //     delay_ms(10);
    // }

    while (1)
    {
        pacer_wait ();
        

        /* Radio checked every tick */
        uint8_t bytes;
        bytes = radio_read_command(rx_buffer, RADIO_PAYLOAD_SIZE);
        if (bytes != 0)
        {
            int left_speed;
            int right_speed;
        
            rx_buffer[bytes] = '\0';
            //printf("RX: %s\n", rx_buffer);
        
            if (sscanf(rx_buffer, "L %d, R %d", &left_speed, &right_speed) == 2)
            {
                printf("Driving: L %d, R %d\n", left_speed, right_speed);
        
                drive_motor(LEFT, left_speed);
                drive_motor(RIGHT, right_speed);
        
                pio_output_toggle(LED_STATUS_PIO);
            }
            else
            {
                printf("Message: %s\n", rx_buffer);
            }
        }

        /* Sleep button polled slowly*/
        button_ticks++;
        if (button_ticks >= BUTTON_TICKS)
        {
            button_ticks = 0;

            button_poll(button_sleep);

            // Enter SAM4S Backup mode, disable various output reducing power.
            if (button_pushed_p(button_sleep)) {
                printf("Button pressed: going to sleep\n");
                fflush(stdout);
                
                drive_motor(LEFT, 0);
                drive_motor(RIGHT, 0);
                led_set(led1, LED_INACTIVE);
                led_set(led2, LED_INACTIVE);
                leds_off(leds);

                sleep_mode_enter_backup(BUTTON_SLEEP_PIO);
            }
        }

        /* Polled very slowly only used for power check*/
        adc_count++;
        if (adc_count >= ADC_TICKS) {
            adc_count = 0;

            adc_read(LED_POW, &adc_raw, sizeof(adc_raw));

            float adc_voltage =  (adc_raw * (3.3f) / 4096.0f) * 26.0f / 11.0f; // 11/26 is voltage divider equation

            if (adc_voltage <= 5.0) {
                //printf("Below 5V: ADC raw: %u, voltage: %.3f V\n", adc_raw, (double) adc_voltage);
                led_set(led2, LED_ACTIVE);
            } else {
                led_set(led2, LED_INACTIVE);
            }
        }

        /* LED flash task. */
        led_ticks++;
        if (led_ticks >= LED_TICKS)
        {
            led_ticks = 0;
            led_toggle(led1);
        }

        /* Rate limited so holding bumper does not transmit every pacer tick*/
        bumper_ticks++;
        if (bumper_ticks >= BUMPER_TX_TICKS)
        {
            bumper_ticks = 0;

            if (pio_input_get(BUMPER_PIO)) {
                if (radio_send_command("buzz"))
                {
                    led_set(led2, LED_INACTIVE);
                }
                else
                {
                    printf("ERROR: TX failed\n");
                    led_set(led2, LED_ACTIVE);
                }
            }
        }
    }
    
    return 0;
}
