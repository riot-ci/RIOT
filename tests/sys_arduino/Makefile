include ../Makefile.tests_common

USEMODULE += arduino

# pull in periph_rtt so that boards providing this feature are using the rtt
# backend of ztimer when using millis and delay functions.
FEATURES_OPTIONAL += periph_rtt

include $(RIOTBASE)/Makefile.include
