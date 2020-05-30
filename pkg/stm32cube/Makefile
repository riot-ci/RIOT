include $(RIOTBASE)/pkg/stm32cube/stm32cube.inc.mk

.PHONY: all stm32cube

include $(RIOTBASE)/pkg/pkg.mk

all: stm32cube

stm32cube:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/$(PKG_NAME).mk
