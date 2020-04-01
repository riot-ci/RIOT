DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

# Handle inclusion of DEFAULT_MODULEs once
USEMODULE := $(USEMODULE) $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))
