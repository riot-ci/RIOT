PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=230f15e3bcd3c84977780e84bd855ac89c1959ee
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	cp src/Makefile $(PKG_BUILDDIR)/Makefile
	cp src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp src/csrc/ucg_riotos.c $(PKG_BUILDDIR)/csrc/ucg_riotos.c
	cp src/csrc/ucg_riotos.h $(PKG_BUILDDIR)/csrc/ucg_riotos.h
	cp src/sys/sdl/dev/Makefile $(PKG_BUILDDIR)/sys/sdl/dev/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
