PKG_NAME=tinycrypt
PKG_URL=https://github.com/01org/tinycrypt
PKG_VERSION=41f67fba4ece1191851d21178167e953ecb85227
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: Makefile.tinycrypt
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/source/ -f $(CURDIR)/Makefile.tinycrypt -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
