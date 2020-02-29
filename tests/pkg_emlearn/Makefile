include ../Makefile.tests_common

USEPKG += emlearn

BLOBS += digit

BUILDDEPS += sonar.h

FEATURES_BLACKLIST += arch_msp430

include $(RIOTBASE)/Makefile.include

sonar.h: $(CURDIR)/model
	$(CURDIR)/generate_model.py
	echo "/* fix for no newline at eof */\n" >> sonar.h
