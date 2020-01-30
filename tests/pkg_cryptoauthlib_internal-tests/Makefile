include ../Makefile.tests_common

# The following boards have too little rom to flash this application
BOARD_BLACKLIST := sam10-xpro saml11-xpro blackpill bluepill \
                   i-nucleo-lrwan1 nucleo-f302r8 nucleo-l031k6

# due to memory constrains we ignore the cert test
CFLAGS += -DDO_NOT_TEST_CERT
USEPKG += cryptoauthlib
USEMODULE += cryptoauthlib_test

include $(RIOTBASE)/Makefile.include
