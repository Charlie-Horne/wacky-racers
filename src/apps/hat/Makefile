COMMON_DIR = ../common

VPATH += $(COMMON_DIR)

PERIPHERALS = pwm pit twi udp adc spi
DRIVERS = panic usb_serial pacer led button nrf24 ledtape adxl345

SRC = hat.c accelerometer.c \
      sleep_mode.c \
      communication.c \
      motor_mapping.c \
      buzzer_test.c \
      leds.c

CFLAGS += -I$(COMMON_DIR)

# Use low level of optimisation to simplify debugging
# OPT = -O1

BOARD = hat

include ../../boards/board.mk