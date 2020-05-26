PKG_NAME=openwsn-fw
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
PKG_VERSION=cbcf622bd9369fcfc8455a5fb9349de2ed3c3a46
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# openwsn_% RIOT Modules or PSEUDOMODULES that don't have custom rules
IGNORE_MODULES  := openwsn_leds \
                   openwsn_debugpins \
                   openwsn_radio \
                   openwsn_serial \
                   openwsn_sctimer \
                   openwsn_cryptoengine \
                   #

OPENWSN_MODULES := $(filter-out $(IGNORE_MODULES),$(filter openwsn_%,$(USEMODULE)))

.PHONY: openwsn_%

all: $(OPENWSN_MODULES)
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

OPENWSN_LOG_LEVEL ?= LOG_NONE
OPENWSN_CFLAGS = -DLOG_LEVEL=$(OPENWSN_LOG_LEVEL)
openwsn_%: CFLAGS+=$(OPENWSN_CFLAGS)

openwsn_openstack:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack -f $(CURDIR)/Makefile.openwsn_module

openwsn_openapps:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openapps -f $(CURDIR)/Makefile.openwsn_module

openwsn_drivers:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/drivers/common -f $(CURDIR)/Makefile.openwsn_module

openwsn_scheduler:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/kernel/openos -f $(CURDIR)/Makefile.openwsn_module

openwsn_cjoin:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openapps/cjoin -f $(CURDIR)/Makefile.openwsn_module

openwsn_opencoap:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openapps/opencoap -f $(CURDIR)/Makefile.openwsn_module

openwsn_mac_low:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/02a-MAClow -f $(CURDIR)/Makefile.openwsn_module

openwsn_mac_high:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/02b-MAChigh -f $(CURDIR)/Makefile.openwsn_module

openwsn_iphc:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/03a-IPHC -f $(CURDIR)/Makefile.openwsn_module

openwsn_ipv6:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/03b-IPv6 -f $(CURDIR)/Makefile.openwsn_module

openwsn_transport:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/04-TRAN -f $(CURDIR)/Makefile.openwsn_module

openwsn_crosslayers:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/openstack/cross-layers -f $(CURDIR)/Makefile.openwsn_module
