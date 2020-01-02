PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=v6.1.1
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

LVGL_MODULES =  \
    lvgl_core   \
    lvgl_draw   \
    lvgl_font   \
    lvgl_hal    \
    lvgl_misc   \
    lvgl_objx   \
    lvgl_themes \
    #

.PHONY: lvgl_%

all: $(LVGL_MODULES)
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)

lvgl_%:
	MODULE=$@ "$(MAKE)" -C $(PKG_BUILDDIR)/src/lv_$* -f $(CURDIR)/Makefile.lvgl_module
