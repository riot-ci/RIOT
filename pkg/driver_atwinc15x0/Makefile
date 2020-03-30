PKG_NAME=driver_atwinc15x0
PKG_URL=https://github.com/arduino-libraries/WiFi101
PKG_VERSION=3301d03f82e53f60e07434a9f07ca677d992d121
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/driver/source -f $(CURDIR)/Makefile.driver_atwinc15x0
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/common/source -f $(CURDIR)/Makefile.driver_atwinc15x0_common
