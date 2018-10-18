include ../Makefile.tests_common

# The following boards are known to fail or have not been tested.
BOARD_BLACKLIST := arduino-mega2560 chronos f4vi1 mega-xplained msb-430 \
                   msb-430h msbiot redbee-econotag stm32f0discovery \
                   stm32f3discovery telosb wsn430-v1_3b wsn430-v1_4 z1 \
                   waspmote-pro

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

USEPKG += relic
USEMODULE += embunit

# -DWORD=32 : 	Specifies the word width of the target system. This is
#  				currently not automatically detected so adjusted to your target
#  				platform.

# The rest of the parameters are configuration parameters for RELIC described in its documentation.
export RELIC_CONFIG_FLAGS=-DARCH=NONE -DOPSYS=NONE -DQUIET=off -DWORD=32 -DFP_PRIME=255 -DWITH="BN;MD;DV;FP;EP;CP;BC;EC" -DSEED=ZERO

include $(RIOTBASE)/Makefile.include
