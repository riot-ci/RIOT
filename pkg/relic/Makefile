PKG_NAME=relic
PKG_URL=https://github.com/tfar/relic.git
PKG_VERSION=947aae085127956580cef5b581622285b1b3818d
PKG_LICENSE=LGPL-2.1

.PHONY: all

all: $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR) && \
	cp $(PKG_BUILDDIR)/lib/librelic_s.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILDDIR)/comp-options.cmake: git-download
	cd "$(PKG_BUILDDIR)" && perl $(PKG_DIR)/generate-cmake-xcompile.perl > comp-options.cmake

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)/comp-options.cmake
	cd "$(PKG_BUILDDIR)" && COMP="$(filter-out -Werror=old-style-definition -Werror=strict-prototypes, $(CFLAGS) ) " cmake -DCMAKE_TOOLCHAIN_FILE=comp-options.cmake -DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS) .

CFLAGS += -Wno-unused-function

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a
include $(RIOTBASE)/pkg/pkg.mk
