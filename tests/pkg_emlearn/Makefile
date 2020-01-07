include ../Makefile.tests_common

USEPKG += emlearn

BLOBS += digit

BUILDDEPS += sonar.h

include $(RIOTBASE)/Makefile.include

sonar.h: $(CURDIR)/model
	$(CURDIR)/generate_model.py
