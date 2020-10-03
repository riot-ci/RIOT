include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe


# define the CAN driver you want to use here
CAN_DRIVER ?= NATIVE

ifeq ($(CAN_DRIVER), NATIVE)

	FEATURES_OPTIONAL += periph_can

else ifeq ($(CAN_DRIVER), MCP2515)
	USEMODULE += mcp2515

else ifeq ($(CAN_DRIVER), CAN_ALT)
	# other can modules can be defined here

endif

include $(RIOTBASE)/Makefile.include
