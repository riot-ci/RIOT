PKG_NAME=driver_llcc68
PKG_URL=https://github.com/Lora-net/llcc68_driver
PKG_VERSION=59b7cd179269f810623f317861a947cf2f4a381e
PKG_LICENSE=BSD

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-enum-conversion

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk
