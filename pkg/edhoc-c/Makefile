PKG_NAME = EDHOC-C
PKG_URL = https://github.com/openwsn-berkeley/EDHOC-C.git
PKG_VERSION = c3e01773d5bceb2ccce2ffe5b9ea083d24615704
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-label
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-incompatible-pointer-types
CFLAGS += -Wno-type-limits
CFLAGS += -Wno-uninitialized
CFLAGS += -Wno-maybe-uninitialized

.PHONY: edhoc-c_%

EDHOC_C_MODULES := $(filter edhoc-c_%,$(USEMODULE))

all: $(EDHOC_C_MODULES)
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=edhoc-c

edhoc-c_crypto_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypto -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c

edhoc-c_cbor_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/cbor -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c
