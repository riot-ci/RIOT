include ../Makefile.tests_common

DRIVER ?= sdp3x

USEMODULE += $(DRIVER)
USEMODULE += xtimer
USEMODULE += periph_i2c
USEMODULE += printf_float
USEMODULE += periph_gpio_irq
USEMODULE += checksum

# export iterations for continuous measurements
CFLAGS += -DTEST_ITERATIONS=$(TEST_ITERATIONS)

include $(RIOTBASE)/Makefile.include
