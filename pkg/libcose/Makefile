PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=5442e1ec94b203414affe7d7f8f1238e10983c3d
PKG_LICENSE=LGPL

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt

include $(RIOTBASE)/pkg/pkg.mk
