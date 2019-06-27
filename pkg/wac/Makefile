PKG_NAME=wac
PKG_URL=https://github.com/kanaka/wac
PKG_VERSION=6
PKG_LICENSE=MPL-2.0

PKG_SOURCE_LOCAL=~/src/wac

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
