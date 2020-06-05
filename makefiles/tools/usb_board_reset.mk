PREFLASH_DELAY ?= 1

ifeq ($(OS),Linux)
  STTY_FLAG = -F
else ifeq ($(OS),Darwin)
  STTY_FLAG = -f
endif

PREFLASHER ?= stty
PREFFLAGS  ?= $(STTY_FLAG) $(PROG_DEV) raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
ifneq (,$(PREFLASH_DELAY))
  FLASHDEPS += preflash-delay
else
  FLASHDEPS += preflash
endif

RESETFFLASG ?= $(STTY_FLAG) $(PORT) raw ispeed 600 ospeed 600 cs8 -cstopb ignpar eol 255 eof 255
RESET ?= $(PREFLASHER) $(RESETFFLASG)

preflash-delay: preflash
	sleep $(PREFLASH_DELAY)
