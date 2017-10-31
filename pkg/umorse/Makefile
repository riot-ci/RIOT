PKG_NAME=umorse
PKG_URL=https://github.com/smlng/uMorse
PKG_VERSION=783ae2191d276c7fcc21a5a2338f907e93b0037c

.PHONY: all

all: git-download
	cp Makefile.umorse $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
