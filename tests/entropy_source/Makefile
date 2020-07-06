DEVELHELP ?= 0
include ../Makefile.tests_common

BOARD ?= native

USEMODULE += fmt
USEMODULE += entropy_source_zero_entropy
USEMODULE += xtimer

# Only include adc noise source if BOARD is not native
ifeq (,$(filter native,$(BOARD)))
  USEMODULE += entropy_source_adc_noise
  # enable ADC health tests and conditioning
  ifndef CONFIG_ENTROPY_SOURCE_ADC_HEALTH_TEST
    CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HEALTH_TEST=1
  endif
  ifndef CONFIG_ENTROPY_SOURCE_ADC_HEALTH_TEST
    CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_COND=1
  endif
endif

include $(RIOTBASE)/Makefile.include
