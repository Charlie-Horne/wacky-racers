/* File:   communication.h
   Author: Dominic McNulty & Cade Hinman
   Date:   11 May 2026
   Descr:  Radio communication used by both Wacky Hat and Wacky Racer.
*/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include "pio.h"

#define RADIO_DEFAULT_CHANNEL 67
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 32

#define RADIO_POWER_ENABLE_PIO IDLE_EN_PIO

void initialize_radio(void);
int serial_read_line(char *buffer, uint8_t buffer_size);
uint8_t radio_read_command(void *buffer, uint8_t radio_size);
int radio_send_command(const char *message);
void communication_print_config(void);

#endif
