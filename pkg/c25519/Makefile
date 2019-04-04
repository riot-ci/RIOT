PKG_NAME    = c25519
PKG_URL     = https://www.dlbeer.co.nz/downloads
PKG_VERSION = 2017-10-05
PKG_EXT     = zip
PKG_LICENSE = PD
PKG_SHA512  = dbfb4285837ab2ea3d99c448b22877cc7a139ccbaebb1de367e2bec1fd562fe629b389d86603915448078b8fd7e631c8fc9a7d126eb889a1ba0c17611369b190

PKG_SHAFILE ?= $(PKGDIRBASE)/$(PKG_NAME)-sha512.txt
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean distclean

prepare: $(PKG_BUILDDIR)/

all: $(PKG_BUILDDIR)/
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.c25519

$(PKGDIRBASE):
	mkdir -p $(PKGDIRBASE)

$(PKG_SHAFILE): $(PKGDIRBASE)
	$(Q)echo "$(PKG_SHA512) $(PKGDIRBASE)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)" > $@

$(PKG_BUILDDIR)/: $(PKGDIRBASE)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	$(Q)$(UNZIP_HERE) -D -d $(PKGDIRBASE) $<

$(PKGDIRBASE)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT): $(PKG_SHAFILE)
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	sha512sum --check $(PKG_SHAFILE)

clean::
	# Reset package to checkout state.
	rm -rf $(PKG_BUILDDIR)

distclean::
	rm -rf $(PKG_BUILDDIR) $(PKGDIRBASE)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	rm -rf $(PKG_SHAFILE)
