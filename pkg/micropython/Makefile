PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=120519194c1cc989f5fb72fb8ae6fba40f2841a5
PKG_LICENSE=MIT

PKG_SOURCE_LOCAL = ~/src/micropython

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

.PHONY: all

all:
	@mkdir -p $(PKG_BUILDDIR)/tmp
	BUILD=$(PKG_BUILDDIR) "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot

include $(RIOTBASE)/pkg/pkg.mk
