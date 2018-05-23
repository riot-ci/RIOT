PKG_NAME=ndn-riot
PKG_URL=https://github.com/astralien3000/ndn-riot
PKG_VERSION=1501308b046dfcb12ad7fa4f0e944663f0d312a7
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
