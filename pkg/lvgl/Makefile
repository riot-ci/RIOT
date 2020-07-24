PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=57079452b0da9b9072fa88da8354e020cc6b6e6b # v7.2.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

LVGL_MODULES =      \
    lvgl_core       \
    lvgl_draw       \
    lvgl_font       \
    lvgl_hal        \
    lvgl_misc       \
    lvgl_themes     \
    lvgl_widgets    \
    #

.PHONY: lvgl_%

all: $(LVGL_MODULES)

lvgl_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/lv_$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@
