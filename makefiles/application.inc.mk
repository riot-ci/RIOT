MODULE = $(APPLICATION_MODULE)

DIRS += $(RIOTCPU)/$(CPU) $(BOARDSDIR)/$(BOARD)
DIRS += $(RIOTBASE)/core $(RIOTBASE)/drivers $(RIOTBASE)/sys

include $(RIOTBASE)/Makefile.base
