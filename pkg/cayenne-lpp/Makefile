PKG_NAME=cayenne-lpp
PKG_URL=https://github.com/aabadie/cayenne-lpp
PKG_VERSION=a418dbe186b2535edfb67896da8cf00b94735470
PKG_LICENSE=LGPLv2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
