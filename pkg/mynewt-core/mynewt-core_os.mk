MODULE = mynewt-core_os

SRC := \
  endian.c \
  os_mbuf.c \
  os_mempool.c \
  os_msys.c \
  os_cputime.c \
  os_cputime_pwr2.c \
  #

include $(RIOTBASE)/Makefile.base
