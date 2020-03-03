include ../Makefile.tests_common

USEPKG += emlearn

BLOBS += digit

BUILDDEPS += sonar.h

# Disable hardware FPU for Cortex-M4/7 to prevent crash on these platforms
DISABLE_MODULE += cortexm_fpu

# emlearn is not compatible with MSP430
FEATURES_BLACKLIST += arch_msp430

# The test crashes on samr21-xpro
TEST_ON_CI_BLACKLIST = samr21-xpro

include $(RIOTBASE)/Makefile.include

sonar.h: $(CURDIR)/model
	$(CURDIR)/generate_model.py
	echo "/* fix for no newline at eof */\n" >> sonar.h
