PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=db8b47f74b2a9b16d9485c03a3a467133caedb69
PKG_LICENSE=ISC

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	cd $(PKG_BUILDDIR)/src && cmake -DCCNL_RIOT=1 -DRIOT_CFLAGS="${RIOT_CFLAGS}" . && make
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a ${BINDIR}/ccn-lite.a

include $(RIOTBASE)/pkg/pkg.mk
