DEVELHELP ?= 0
include ../Makefile.tests_common

FEATURES_REQUIRED += periph_pm

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

#DISABLE_MODULE += auto_init

CFLAGS += -DTEST_PM_MODE=2
CFLAGS += -DAUTO_INIT_LED0

# disable assert() and all log output
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

# disable some default modules
DISABLE_MODULE += auto_init core_msg core_thread_flags core_panic

# disable C library and uart output
#DISABLE_MODULE += newlib newlib_nano uart_stdio

include $(RIOTBASE)/Makefile.include
