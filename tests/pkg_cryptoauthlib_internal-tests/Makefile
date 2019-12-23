include ../Makefile.tests_common

INCLUDES += -I$(RIOTBASE)/pkg/cryptoauthlib/include
FEATURES_REQUIRED += periph_i2c
CFLAGS += -std=gnu11 -O3 -Wno-unused-parameter -Wno-missing-field-initializers -Wno-int-conversion -Wno-unused-function -Wno-unused-variable 
CFLAGS += -DATCA_HAL_I2C -DDO_NOT_TEST_CERT
USEPKG += cryptoauthlib
USEMODULE += xtimer cryptoauthlib_test

include $(RIOTBASE)/Makefile.include
