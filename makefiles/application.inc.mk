MODULE = application_$(APPLICATION)

DIRS += $(RIOTCPU)/$(CPU) $(RIOTBOARD)/$(BOARD)
DIRS += $(RIOTBASE)/core $(RIOTBASE)/drivers $(RIOTBASE)/sys

include $(RIOTBASE)/Makefile.base
