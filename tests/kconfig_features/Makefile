include ../Makefile.tests_common

BOARD_WHITELIST += \
	arduino-mkr1000 \
	arduino-mkrfox1200 \
	arduino-mkrwan1300 \
	arduino-mkrzero \
	arduino-zero \
	feather-m0 \
	hamilton \
	samd21-xpro \
	samr21-xpro \
	sensebox_samd21 \
	serpente \
	sodaq-autonomo \
	sodaq-explorer \
	sodaq-one \
	#

kconfig-features:
	@bash -c 'diff <($(MAKE) info-features-provided) \
	    <($(MAKE) dependency-debug-features-provided-kconfig) || \
	    (echo "ERROR: Kconfig features mismatch" && exit 1)'

all: kconfig-features

include $(RIOTBASE)/Makefile.include
