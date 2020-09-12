include ../Makefile.tests_common

USEMODULE += ssp

# This test intentionally crashes the firmware and when using a board with
# stdio over CDC ACM the USB thread is no longer serviced
FEATURES_BLACKLIST += stdio_cdc_acm

include $(RIOTBASE)/Makefile.include

ifneq (,$(shell $(CC) --help=warnings | grep '\-Wstringop-overflow='))
  CFLAGS += -Wstringop-overflow=0
endif
