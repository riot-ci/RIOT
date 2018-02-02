PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=0474080335aac08d3afa44a439ac9def92b9fcf3
PKG_LICENSE=ISC

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

$(info $(PKG_BUILDDIR))
$(info $(RIOTBASE))
$(info $(RIOTTOOLS))
$(info $(PKG_NAME))

all: $(PKG_BUILDDIR)/src/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)/src && \
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a ${BINDIR}/ccn-lite.a

$(PKG_BUILDDIR)/src/Makefile: $(PKG_BUILDDIR)/xcompile-toolchain.cmake
	cd $(PKG_BUILDDIR)/src && \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake \
		  -DCCNL_RIOT=1 -DRIOT_CFLAGS="${RIOT_CFLAGS}" .

$(PKG_BUILDDIR)/xcompile-toolchain.cmake: git-download
	cd $(PKG_BUILDDIR) && \
	perl $(RIOTTOOLS)/cmake/generate-xcompile-toolchain.pl > xcompile-toolchain.cmake

include $(RIOTBASE)/pkg/pkg.mk
