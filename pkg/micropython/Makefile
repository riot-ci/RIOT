PKG_NAME=micropython
PKG_URL=https://github.com/micropython/micropython
PKG_VERSION=c3f519adfe1faf7808fa49fdb6006040317279ca

.PHONY: all

all: git-download
	BUILD=$(PKG_BUILDDIR) CFLAGS="$${CFLAGS} -Wno-error" "$(MAKE)" -C $(PKG_BUILDDIR)/riot

include $(RIOTBASE)/pkg/pkg.mk
