PKG_NAME=mynewt-core
PKG_URL=https://github.com/apache/mynewt-core.git
PKG_VERSION=299b8eeadde2c3e434599d017d87080668b6a8c6
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-sign-compare

MYNEWT_CORE_MODULES := mynewt-core_os \
                       mynewt-core_util \
                       mynewt-core_nrf5x_hal \
                       #

MYNEWT_CORE_PATH_util      = util/mem/src
MYNEWT_CORE_PATH_os        = kernel/os/src

ifneq (,$(filter nrf52,$(CPU)))
  MYNEWT_CORE_PATH_nrf5x_hal = hw/mcu/nordic/nrf52xxx/src/
endif
ifneq (,$(filter nrf51,$(CPU)))
  MYNEWT_CORE_PATH_nrf5x_hal = hw/mcu/nordic/nrf51xxx/src/
endif

all: $(filter $(MYNEWT_CORE_MODULES),$(USEMODULE))
	@true

mynewt-core_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(MYNEWT_CORE_PATH_$*) -f $(RIOTPKG)/$(PKG_NAME)/$@.mk MODULE=$@
