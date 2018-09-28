PKG_NAME=littlefs
PKG_URL=https://github.com/geky/littlefs.git
# v1.6.1
PKG_VERSION=cb62bf2188854c5b7c44383571ebb19a414e6137
PKG_LICENSE=Apache-2.0

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs

include $(RIOTBASE)/pkg/pkg.mk
