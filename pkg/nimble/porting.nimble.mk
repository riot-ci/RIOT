MODULE = nimble_porting_nimble

SRC += nimble_port.c
SRC += endian.c
SRC += mem.c
SRC += os_mbuf.c
SRC += os_mempool.c
SRC += os_msys_init.c

ifneq (,$(filter nimble_controller,$(USEMODULE)))
  SRC += os_cputime.c
  SRC += os_cputime_pwr2.c
  SRC += hal_timer.c
endif

include $(RIOTBASE)/Makefile.base
