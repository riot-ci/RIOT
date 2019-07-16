PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=666297cc1cdc39020b048614bd501a4c7f7f5f3c
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

.PHONY: all

all:
	@mkdir -p $(PKG_BUILDDIR)/tmp
	BUILD=$(PKG_BUILDDIR) "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot

include $(RIOTBASE)/pkg/pkg.mk
