include ../Makefile.tests_common

USEMODULE += suit
USEMODULE += riotboot_hdr
USEMODULE += embunit

# Lots of structs on the stack and crypto verification
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(6*THREAD_STACKSIZE_DEFAULT\)

BLOBS += $(wildcard manifests/manifest*.bin)
CFLAGS += -g -DDEVELHELP -DLOG_LEVEL=LOG_DEBUG

USEMODULE += suit_transport_mock

ifeq (native, $(BOARD))
  # Fake some flash support
  CFLAGS += -DFLASHPAGE_SIZE=256 -DFLASHPAGE_NUMOF=16
  FEATURES_PROVIDED += periph_flashpage
endif

FEATURES_REQUIRED += periph_flashpage

BUILDDEPS += create_test_data

include $(RIOTBASE)/Makefile.include

export SUIT_TOOL
export SUIT_SEC

create_test_data: $(SUIT_SEC)
	@mkdir -p manifests
	sh create_test_data.sh
