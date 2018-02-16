PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=1f549a3496ac543390a170a7eb7b242475063016
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter

.PHONY: all

all: git-download
	BUILD=$(PKG_BUILDDIR) CFLAGS="$${CFLAGS} -Wno-error" "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot

include $(RIOTBASE)/pkg/pkg.mk
