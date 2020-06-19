include ../Makefile.tests_common

USEPKG += mbedtls

# You can disable single entropy source to be used with the entropy module
# DISABLE_MODULE += mbedtls_entropy_source_hwrng

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter mbedtls_entropy_source_adc,$(USEMODULE)))
	ifndef CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE
		CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HMIN=65536
	endif
endif
