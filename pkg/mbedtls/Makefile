PKG_NAME=mbedtls
PKG_URL=https://github.com/ARMmbed/mbedtls.git
PKG_VERSION=2a1d9332d55d1270084232e42df08fdb08129f1b
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKGDIRBASE)/mbedtls/library/ \
			  -f $(RIOTPKG)/mbedtls/Makefile.$(PKG_NAME)
