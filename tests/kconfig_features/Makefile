include ../Makefile.tests_common

BOARD_WHITELIST += arduino-duemilanove \
                   arduino-nano \
                   arduino-uno \
                   atmega328p \
                   samr21-xpro

all: kconfig-features

include $(RIOTBASE)/Makefile.include

kconfig-features: $(KCONFIG_OUT_CONFIG)
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'
