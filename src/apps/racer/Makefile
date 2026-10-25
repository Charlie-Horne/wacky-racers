COMMON_DIR = ../common

VPATH += $(COMMON_DIR)

PERIPHERALS = pwm pit adc spi 
DRIVERS = panic usb_serial pacer led button nrf24 ledtape

SRC = racer.c \
      pwm_driver.c \
      sleep_mode.c \
      communication.c \
      leds.c \

CFLAGS += -I$(COMMON_DIR)

# Use low level of optimisation to simplify debugging
#OPT = -O1

BOARD = racer

include ../../boards/board.mk