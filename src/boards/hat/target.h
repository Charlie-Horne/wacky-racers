/** @file   target.h
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a hat!  */

/* System clocks  */
#define F_XTAL 12.00e6
#define MCU_PLL_MUL 16
#define MCU_PLL_DIV 1

#define MCU_USB_DIV 2
/* 192 MHz  */
#define F_PLL (F_XTAL / MCU_PLL_DIV * MCU_PLL_MUL)
/* 96 MHz  */
#define F_CPU (F_PLL / 2)

/* TWI  */
#define TWI_TIMEOUT_US_DEFAULT 10000
#define MPU_ADDRESS 0x68

/* USB  */
//#define USB_VBUS_PIO PA5_PIO
#define USB_CURRENT_MA 500



/* Accelerometer  */

#define ADXL345_ADDRESS 0x53
#define ACCELEROMETER_SCL_PIO PA4_PIO
#define ACCELEROMETER_SDA_PIO PA3_PIO
#define ACCELEROMETER_INT1_PIO PA28_PIO
#define ACCELEROMETER_INT2_PIO PA27_PIO


/* LEDs  */
#define LED_ERROR_PIO PA0_PIO
#define LED_STATUS_PIO PA1_PIO
#define LED_ACTIVE 0
#define LED_INACTIVE 1



/* General  */
#define APPENDAGE_PIO PA1_PIO
#define BUZZER_PWM_PIO PA26_PIO

/* Button  */
#define BUTTON_SLEEP_PIO PA5_PIO

/* Radio  */
#define RADIO_IRQ_PIO PA9_PIO
#define RADIO_CE_PIO PA10_PIO
#define RADIO_CS_PIO PA11_PIO
#define RADIO_MISO_PIO PA12_PIO
#define RADIO_MOSI_PIO PA13_PIO
#define RADIO_SPCK_PIO PA14_PIO
#define IDLE_EN_PIO PA8_PIO

/* Radio Channels*/
#define CHANNEL_SELECT_0 PB0_PIO
#define CHANNEL_SELECT_1 PB1_PIO
#define CHANNEL_SELECT_2 PB2_PIO
#define CHANNEL_SELECT_3 PB3_PIO

/* LED_POW PIN*/
#define LED_POW_CHANNEL ADC_CHANNEL_1

/* LED tape  */
#define LEDTAPE_PIO PA7_PIO

#endif /* TARGET_H  */
