PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=4544e8f4694a7428d7246daac7a80e306826eea7
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter

.PHONY: all

all: git-download
	BUILD=$(PKG_BUILDDIR) CFLAGS="$${CFLAGS} -Wno-error" "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot

include $(RIOTBASE)/pkg/pkg.mk
