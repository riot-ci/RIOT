PKG_NAME=ndn-riot
PKG_URL=https://github.com/astralien3000/ndn-riot
PKG_VERSION=c12d057eca51b9676d8c423e523286684636be08
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
