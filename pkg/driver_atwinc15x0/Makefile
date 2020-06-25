PKG_NAME=driver_atwinc15x0
PKG_URL=https://github.com/arduino-libraries/WiFi101
PKG_VERSION=3301d03f82e53f60e07434a9f07ca677d992d121
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-discarded-qualifiers
CFLAGS += -Wno-empty-body
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-unused-parameter

CFLAGS += -DETH_MODE

all:
	"$(MAKE)" MODULE=driver_atwinc15x0 -C $(PKG_BUILDDIR)/src/driver/source -f $(RIOTBASE)/Makefile.base
	"$(MAKE)" MODULE=driver_atwinc15x0_common -C $(PKG_BUILDDIR)/src/common/source -f $(RIOTBASE)/Makefile.base
	"$(MAKE)" MODULE=driver_atwinc15x0_spi_flash -C $(PKG_BUILDDIR)/src/spi_flash/source -f $(RIOTBASE)/Makefile.base
