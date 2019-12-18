include ../Makefile.tests_common

FEATURES_REQUIRED += periph_i2c
CFLAGS += -std=gnu11 -Wno-unused-parameter -DATCA_HAL_I2C
USEPKG += cryptoauthlib
USEMODULE += xtimer
USEMODULE += auto_init_security
USEMODULE += hashes

include $(RIOTBASE)/Makefile.include