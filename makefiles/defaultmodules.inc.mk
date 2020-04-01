DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

-include $(BOARDSDIR)/$(BOARD)/defaultmodules.inc.mk
-include $(RIOTCPU)/$(CPU)/defaultmodules.inc.mk

# Handle inclusion of DEFAULT_MODULEs once
USEMODULE := $(USEMODULE) $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))
