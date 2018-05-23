PKG_NAME=ndn-riot
PKG_URL=https://github.com/astralien3000/ndn-riot
PKG_VERSION=eaa8372537b9d787e8acc9a72972b4fd4d27ee3a
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
