/* File:   hat.c
   Descr:  Wacky Hat accelerometer to motor command radio transmitter.
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
#include "accelerometer.h"
#include "motor_mapping.h"
#include "communication.h"
#include "sleep_mode.h"
#include "buzzer_test.h"
#include "leds.h"

#define LED_FLASH_RATE 2
/* Define LED configuration.  */

/* Main scheduler tick rate. */
#define PACER_RATE 500

/* Scheduled task rates in Hz. */
#define LED_FLASH_RATE 2
#define BUTTON_POLL_RATE 20
#define ADC_POLL_RATE 1
#define HAT_TX_RATE 10


#define LED_TICKS       (PACER_RATE / (LED_FLASH_RATE * 2))
#define BUTTON_TICKS    (PACER_RATE / BUTTON_POLL_RATE)
#define ADC_TICKS       (PACER_RATE / ADC_POLL_RATE)
#define HAT_TX_DIVISOR (PACER_RATE / HAT_TX_RATE)

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

// Sleep Button
static button_t button_sleep;

// Low power ADC input
static adc_t LED_POW;
static uint16_t adc_raw;
static uint16_t adc_count = 0;


//LEDs
static led_t led1;
static led_t led2;
static ledbuffer_t *leds;

//accelerometer
adxl345_t *accelerometer;

int initialize_peripherals(void) 
{
    mcu_jtag_disable();

    // Redirect stdio to USB serial
    if (usb_serial_stdio_init () < 0)
    panic(LED_STATUS_PIO, 4);

    LED_POW = adc_init(&adc_cfg);

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

int main(void)
{
    initialize_peripherals();

    
    int tx_count = 0;

    uint32_t led_ticks = 0;
    uint32_t button_ticks = 0;

    char rx_buffer[RADIO_PAYLOAD_SIZE + 1];

    accelerometer = accelerometer_init();

    while (1)
    {
        pacer_wait();

        /* Radio checked every tick */
        uint8_t bytes;
        bytes = radio_read_command(rx_buffer, RADIO_PAYLOAD_SIZE);
        if (bytes != 0)
        {
            rx_buffer[bytes] = '\0';
            printf("RX: %s\n", rx_buffer);
            
            if (strcmp(rx_buffer, "buzz") == 0)
            {
                pio_output_toggle(LED_ERROR_PIO);
            }
        }

        /* LED flash task. */
        led_ticks++;
        if (led_ticks >= LED_TICKS)
        {
            led_ticks = 0;
            led_toggle(led1);
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
                
                led_set(led1, LED_INACTIVE);
                led_set(led2, LED_INACTIVE);
                leds_off(leds);
                pio_config_set(BUZZER_PWM_PIO, PIO_OUTPUT_LOW);

                sleep_mode_enter_backup(BUTTON_SLEEP_PIO);
            }
        }

        /* Polled very slowly only used for power check*/
        adc_count++;
        if (adc_count >= ADC_TICKS) {
            adc_count = 0;

            adc_read(LED_POW, &adc_raw, sizeof(adc_raw));

            float adc_voltage =  (adc_raw * (3.3f) / 4096.0f) * 26.0f / 11.0f; // 11/26 is voltage divider equation

            printf("ADC raw: %u, voltage: %.3f V\n", adc_raw, (double) adc_voltage);

            if (adc_voltage <= 5.0) {
                led_set(led2, LED_ACTIVE);
            } else {
                led_set(led2, LED_INACTIVE);
            }
        }

        tx_count++;
        if (tx_count >= HAT_TX_DIVISOR)
        {
            tx_count = 0;
        
            if (accelerometer_is_ready(accelerometer))
            {
                accel_data_t accel;
                motor_command_t motors;
        
                if (accelerometer_read(accelerometer, &accel))
                {
                    motors = motor_mapping_from_acc(accel.x, accel.y);
        
                    char message[RADIO_PAYLOAD_SIZE];
                    snprintf(message, sizeof(message), "L %4d, R %4d",
                             motors.left, motors.right);
        
                    radio_send_command(message);
                }
            }
        }
    }
    return 0;
}
