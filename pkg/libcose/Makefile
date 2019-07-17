PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=f92297b994892a676595280b500809654485bda1
PKG_LICENSE=LGPL

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt

include $(RIOTBASE)/pkg/pkg.mk
