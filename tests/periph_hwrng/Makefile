include ../Makefile.tests_common

FEATURES_REQUIRED = periph_hwrng

USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

# samr30 does not contain a HWRNG
# TODO: make use of the HWRNG on AT86RF212B
BOARD_BLACKLIST := samr30-xpro

include $(RIOTBASE)/Makefile.include
