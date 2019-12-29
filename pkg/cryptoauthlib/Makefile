PKG_NAME=cryptoauthlib
PKG_URL=https://github.com/MicrochipTech/cryptoauthlib
PKG_VERSION=af8187776cd3f3faf8bed412eaf6ff7221862e19
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)
TESTINCLDIR = $(PKG_BUILDDIR)/test

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-function -Wno-type-limits -Wno-strict-aliasing
TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake

ifneq (,$(filter cryptoauthlib_test,$(USEMODULE)))
all: cryptoauth test
else
all: cryptoauth
endif

cryptoauth: $(PKG_BUILDDIR)/lib/Makefile
	CFLAGS="$(filter-out -Werror -Werror=old-style-definition -Werror=strict-prototypes -Werror=pedantic -Wpedantic -pedantic -std=gnu99, $(CFLAGS) ) " \
	$(MAKE) -C $(PKG_BUILDDIR)/lib && \
	cp $(PKG_BUILDDIR)/lib/libcryptoauth.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILDDIR)/lib/Makefile: $(TOOLCHAIN_FILE)
	cd $(PKG_BUILDDIR)/lib && CC="$(CC)" CXX="$(CXX)" \
	CFLAGS="$(filter-out -Werror -Werror=old-style-definition -Werror=strict-prototypes -Werror=pedantic -Wpedantic -pedantic -std=gnu99, $(CFLAGS) ) " \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -Wno-dev \
		  -DBUILD_TESTS=OFF \
		  -DATCA_HAL_I2C:BOOL=TRUE .

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

test:
	cp $(TESTINCLDIR)/*.c $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/jwt/*.c $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/tng/*.c $(PKG_BUILDDIR)
	cp $(TESTINCLDIR)/atcacert/*.c $(PKG_BUILDDIR)

	echo "MODULE = cryptoauthlib_test" > $(PKG_BUILDDIR)/Makefile
	echo "include \$$(RIOTBASE)/Makefile.base" >> $(PKG_BUILDDIR)/Makefile

	"$(MAKE)" -C $(PKG_BUILDDIR)

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a

include $(RIOTBASE)/pkg/pkg.mk
