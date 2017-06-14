PKG_NAME     = bossa
PKG_URL      = https://github.com/shumatech/BOSSA
PKG_VERSION  = 3be622ca0aa6214a2fc51c1ec682c4a58a423d62
PKG_LICENSE  = BSD-3-Clause
PKG_BUILDDIR = $(CURDIR)/bin

.PHONY: all

all: git-download
	@echo "[INFO] compiling bossac from source now"
	@env -u CXX COMMON_CXXFLAGS="-std=c++11" $(MAKE) BINDIR=$(PKG_BUILDDIR) -C $(PKG_BUILDDIR) strip-bossac
	@mv $(PKG_BUILDDIR)/bossac $(CURDIR)/bossac

distclean::
	@rm -f $(CURDIR)/bossac

include $(RIOTBASE)/pkg/pkg.mk
