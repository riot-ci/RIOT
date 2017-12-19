PKG_NAME=littlefs
PKG_URL=https://github.com/geky/littlefs.git
PKG_VERSION=5ee20e8d774adf0bb538269870b3552fdfc0e046
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)

.PHONY: all

all: git-download
	@mkdir -p "$(PKG_BUILDDIR)/riotbuild"
	@cp $(PKG_BUILDDIR)/*.c $(PKG_BUILDDIR)/*.h $(PKG_BUILDDIR)/riotbuild
	@cp $(CURDIR)/Makefile.littlefs $(PKG_BUILDDIR)/riotbuild/Makefile

	"$(MAKE)" -C $(PKG_BUILDDIR)/riotbuild

include $(RIOTBASE)/pkg/pkg.mk
