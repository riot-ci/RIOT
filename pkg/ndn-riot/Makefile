PKG_NAME=ndn-riot
PKG_URL=https://github.com/astralien3000/ndn-riot
PKG_VERSION=9466eff546d0b09b9d539e12130efc7640dda8fd
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
