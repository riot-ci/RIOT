PKG_NAME=micropython
PKG_URL=https://github.com/micropython/micropython
PKG_VERSION=c3ae03ff18a9236c3b98efcfb581272bc56e292e

.PHONY: all

all: git-download
	BUILD=$(PKG_BUILDDIR) "$(MAKE)" -C $(PKG_BUILDDIR)/riot

include $(RIOTBASE)/pkg/pkg.mk
