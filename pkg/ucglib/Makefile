PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=bf48515702dd7c87cbacdd17989738f33f003df2
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	cp Makefile.ucglib $(PKG_BUILDDIR)/Makefile
	cp Makefile.ucglib_csrc $(PKG_BUILDDIR)/csrc/Makefile
	cp Makefile.ucglib_sdl $(PKG_BUILDDIR)/sys/sdl/dev/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
