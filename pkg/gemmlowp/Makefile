PKG_NAME=gemmlowp
PKG_URL=https://github.com/google/gemmlowp
PKG_VERSION=dc69acdf61d7a64260ae0eb9c17421fef0488c02
PKG_LICENSE=Apache2.0

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
