PKG_NAME=tinycrypt
PKG_URL=https://github.com/01org/tinycrypt
PKG_VERSION=8a35f172d48aebbdf3adc4b69af10b0957f1a8e0
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
