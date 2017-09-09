DEFAULT_MODULE += board cpu core core_msg sys

DEFAULT_MODULE += auto_init

# all boards use gpio for leds
ifneq (,$(filter periph_gpio,$(FEATURES_PROVIDED)))
  DEFAULT_MODULE += periph_gpio
endif

# include power management
ifneq (,$(filter periph_pm,$(FEATURES_PROVIDED)))
  DEFAULT_MODULE += periph_pm
endif

USEMODULE += $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))
