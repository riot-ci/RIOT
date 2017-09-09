DEFAULT_MODULE += board cpu core core_msg sys

DEFAULT_MODULE += auto_init

define use_periph_module_if_available =
  ifneq (,$$(filter ${1},$(FEATURES_PROVIDED)))
    DEFAULT_MODULE += ${1}
  endif
endef

# all boards use gpio for leds
$(eval $(call use_periph_module_if_available,periph_gpio))

# include power management
$(eval $(call use_periph_module_if_available,periph_pm))

# include uart support
$(eval $(call use_periph_module_if_available,periph_uart))

USEMODULE += $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))
