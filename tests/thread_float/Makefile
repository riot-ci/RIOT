APPLICATION = thread_float
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon calliope-mini cc2650stk chronos maple-mini \
                             mbed_lpc1768 microbit msb-430 msb-430h nrf51dongle \
                             nrf6310 nucleo32-f031 nucleo32-f042 nucleo32-f303 \
                             nucleo-f030 nucleo-f070 nucleo-f072 nucleo-f103 \
                             nucleo-f334 opencm9-04 pca10000 pca10005 spark-core \
                             stm32f0discovery weio yunjia-nrf51822

USEMODULE += printf_float
USEMODULE += xtimer

#DISABLE_MODULE += cortexm_fpu

include $(RIOTBASE)/Makefile.include
