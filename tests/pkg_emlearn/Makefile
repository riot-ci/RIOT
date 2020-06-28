include ../Makefile.tests_common

USEPKG += emlearn

BLOBS += digit

BUILDDEPS += model.h

# The test crashes on samr21-xpro
TEST_ON_CI_BLACKLIST = samr21-xpro

include $(RIOTBASE)/Makefile.include

model.h: $(CURDIR)/model
	$(Q)$(CURDIR)/generate_model.py
