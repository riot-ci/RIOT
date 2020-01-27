APPLICATION = candev
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += can

# define the CAN driver you want to use here
CAN_DRIVER ?= MCP2515

ifeq ($(CAN_DRIVER), MCP2515)
    FEATURES_REQUIRED = periph_gpio periph_spi periph_gpio_irq

    USEMODULE += mcp2515
else ifeq ($(CAN_DRIVER), PERIPH_CAN)
# periph_can modules/variables go here

else ifeq ($(CAN_DRIVER), CAN_NATIVE)
# can_native modules/variables go here

endif

CFLAGS += -DCAN_DRIVER_$(CAN_DRIVER)

include $(RIOTBASE)/Makefile.include
