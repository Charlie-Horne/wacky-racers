/** @file   target.h
    @date   12 February 2018
    @brief
*/
#ifndef TARGET_H
#define TARGET_H

#include "mat91lib.h"

/* This is for the carhat (chart) board configured as a racer!  */

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

/* USB  */
//#define USB_VBUS_PIO PA5_PIO
#define USB_CURRENT_MA 500

/* LEDs  */
#define LED_ERROR_PIO PA27_PIO
#define LED_STATUS_PIO PA28_PIO
#define LED_ACTIVE 0
#define LED_INACTIVE 1

/* General  */
#define APPENDAGE_PIO PA1_PIO
#define SERVO_PWM_PIO PA2_PIO

/* Button  */
#define BUTTON_SLEEP_PIO PA5_PIO

/* H-bridges   */
#define MOTOR_L1_PWM_PIO PA15_PIO //AIN1
#define MOTOR_L2_PWM_PIO PA16_PIO //AIN2

#define MOTOR_R1_PWM_PIO PA19_PIO //BIN1
#define MOTOR_R2_PWM_PIO PA20_PIO //BIN2

/* Radio  */
#define RADIO_IRQ_PIO PA9_PIO
#define RADIO_CE_PIO PA10_PIO
#define RADIO_CS_PIO PA11_PIO
#define RADIO_MISO_PIO PA12_PIO
#define RADIO_MOSI_PIO PA13_PIO
#define RADIO_SPCK_PIO PA14_PIO

/* Radio Channels*/
#define CHANNEL_SELECT_0 PB0_PIO
#define CHANNEL_SELECT_1 PB1_PIO
#define CHANNEL_SELECT_2 PB2_PIO
#define CHANNEL_SELECT_3 PB3_PIO

/* LED tape  */
#define LEDTAPE_PIO PA7_PIO

/* IDLE_EN  */
#define IDLE_EN_PIO PA8_PIO

/* LED_POW PIN*/
#define LED_POW_CHANNEL ADC_CHANNEL_1

/* BUMPER PIN */
#define BUMPER_PIO PB13_PIO

#endif /* TARGET_H  */
