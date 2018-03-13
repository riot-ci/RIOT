PKG_NAME=tweetnacl
PKG_URL=https://github.com/kaspar030/tweetnacl
PKG_VERSION=2bb06b428782909bc37e47eff11e461832c19c45
PKG_LICENSE=PD

.PHONY: all

all: git-download
	@cp $(RIOTBASE)/pkg/tweetnacl/src/* $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
