PKG_NAME=cryptoauthlib_test
PKG_SOURCE_LOCAL=$(PKGDIRBASE)/cryptoauthlib/test
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)
TESTINCLDIR = $(PKGDIRBASE)/cryptoauthlib_test

INCLUDES+= -I$(TESTINCLDIR)
INCLUDES+= -I$(TESTINCLDIR)/jwt
INCLUDES+= -I$(TESTINCLDIR)/sha-byte-test-vectors
INCLUDES+= -I$(TESTINCLDIR)/tng
INCLUDES+= -I$(TESTINCLDIR)/aes_gcm_cavp_vectors
INCLUDES+= -I$(TESTINCLDIR)/atcacert

.PHONY: all..cmake_version_supported

all: patch
	$(info $$CFLAGS is [${CFLAGS}])
	"$(MAKE)" -C $(PKG_BUILDDIR)

patch: git-download
	mkdir -p $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/jwt/*.c $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/tng/*.c $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/atcacert/*.c $(PKG_BUILDDIR)

	echo "MODULE = cryptoauthlib_test" > $(PKG_BUILDDIR)/Makefile
	echo "include \$$(RIOTBASE)/Makefile.base" >> $(PKG_BUILDDIR)/Makefile

include $(RIOTBASE)/pkg/pkg.mk