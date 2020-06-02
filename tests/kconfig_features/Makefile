include ../Makefile.tests_common

BOARD_WHITELIST += \
	arduino-mkr1000 \
	arduino-mkrfox1200 \
	samr21-xpro \
	#

kconfig-features:
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

all: kconfig-features

include $(RIOTBASE)/Makefile.include
