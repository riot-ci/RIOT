PKG_NAME=ndn-riot
PKG_URL=https://github.com/astralien3000/ndn-riot
PKG_VERSION=3180d61e16e59aa572cd5f687d80ed519c08343d
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
