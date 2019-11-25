PKG_NAME=micropython
PKG_URL=https://github.com/RIOT-OS/micropython
PKG_VERSION=55b0385ae965ceceeb719aa1a5563b2facf7318c
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

.PHONY: all

all:
	@mkdir -p $(PKG_BUILDDIR)/tmp
	BUILD=$(PKG_BUILDDIR) "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot

include $(RIOTBASE)/pkg/pkg.mk
