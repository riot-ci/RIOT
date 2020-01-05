PKG_NAME=flatbuffers
PKG_URL=https://github.com/google/flatbuffers
PKG_VERSION=v1.11.0
PKG_LICENSE=Apache2.0

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
