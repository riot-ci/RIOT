BOARD ?= arduino-zero

include ../Makefile.tests_common

USEMODULE += arduino

# Features used by Arduino analogRead/analogWrite functions are required
FEATURES_REQUIRED += arduino_pwm
FEATURES_REQUIRED += periph_adc

include $(RIOTBASE)/Makefile.include
