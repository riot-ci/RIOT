PKG_NAME=nanopb
PKG_URL=https://github.com/nanopb/nanopb
PKG_VERSION=3c69a905b16df149e1bda12f25e0522073a24678 # nanopb-0.3.9.3
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.nanopb

include $(RIOTBASE)/pkg/pkg.mk
