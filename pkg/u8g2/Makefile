PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=51ca3f942fe38fda86ea6779390e25c326701bc7
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
