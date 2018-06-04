PKG_NAME=gecko_sdk
PKG_URL=https://github.com/kaibeckmann/RIOT-gecko-sdk
PKG_VERSION=c4be44b6a57897d00c3e00f9084d697bd011f73a
PKG_LICENSE=Zlib

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

CFLAGS += -Wno-int-in-bool-context

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/dist

include $(RIOTBASE)/pkg/pkg.mk
