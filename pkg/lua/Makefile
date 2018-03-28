PKG_NAME=lua
PKG_URL=https://github.com/lua/lua.git
PKG_VERSION=v5-3-4
PKG_LICENSE=MIT
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all:
	@cp Makefile.lua $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR) -f Makefile.lua

include $(RIOTBASE)/pkg/pkg.mk
