SRC := $(filter-out lua.c luac.c,$(wildcard *.c))


include $(RIOTBASE)/Makefile.base
