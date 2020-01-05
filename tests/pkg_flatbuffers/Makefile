include ../Makefile.tests_common

USEPKG += flatbuffers

BUILDDEPS += monster_generated.h

include $(RIOTBASE)/Makefile.include

monster_generated.h: $(FLATC)
	$(Q)$(FLATC) --cpp $(CURDIR)/monster.fbs
