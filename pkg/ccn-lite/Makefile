PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=48b17ebee7600b2e79b3acf0728217473d7a4ee8
PKG_LICENSE=ISC

.PHONY: all ..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0


RIOT_CFLAGS = $(INCLUDES)

TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake

all: $(PKG_BUILDDIR)/src/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)/src && \
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a $(BINDIR)/ccn-lite.a

$(PKG_BUILDDIR)/src/Makefile: $(TOOLCHAIN_FILE)
	cd $(PKG_BUILDDIR)/src && \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -DCCNL_RIOT=1 -DRIOT_CFLAGS="$(RIOT_CFLAGS)" -DBUILD_TESTING=OFF .
$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

git-download: | ..cmake_version_supported

# Verify that current cmake version is higher than CMAKE_MINIMAL_VERSION
# Adapted from https://stackoverflow.com/a/24067243 to have >=
..cmake_version_supported:
	@ # Remove '-rcX' from version as 'sort -V' does not handle them well
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	HIGHEST=$$(printf "$${CMAKE_VERSION}\n$(CMAKE_MINIMAL_VERSION)\n" | sort -rV | head -n 1); \
	test "$${HIGHEST}" = "$${CMAKE_VERSION}" || \
	{ printf "\ncmake version $${CMAKE_VERSION} is not >= to minimal required $(CMAKE_MINIMAL_VERSION)\n\n"; exit 1; }


include $(RIOTBASE)/pkg/pkg.mk
ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
