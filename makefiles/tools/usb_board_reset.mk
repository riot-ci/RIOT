ifneq (,$(filter reset flash flash-only, $(MAKECMDGOALS)))
  # By default, add 2 seconds delay before opening terminal: this is required
  # when opening the terminal right after flashing. In this case, the stdio
  # over USB needs some time after reset before being ready.
  TERM_DELAY ?= 2
  TERMDEPS += term-delay
  TERMDELAYDEPS = $(filter reset flash flash-only, $(MAKECMDGOALS))
endif

ifeq ($(OS),Linux)
  STTY_FLAG = -F
else ifeq ($(OS),Darwin)
  STTY_FLAG = -f
endif

PREFLASHER ?= stty
PREFFLAGS  ?= $(STTY_FLAG) $(PROG_DEV) raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
PREFLASH_DELAY ?= 1
FLASHDEPS += preflash-delay

RESETFFLASG ?= $(STTY_FLAG) $(PORT) raw ispeed 600 ospeed 600 cs8 -cstopb ignpar eol 255 eof 255
RESET ?= $(PREFLASHER) $(RESETFFLASG)

TESTRUNNER_CONNECT_DELAY ?= $(TERM_DELAY)
$(call target-export-variables,test,TESTRUNNER_CONNECT_DELAY)

preflash-delay: preflash
	sleep $(PREFLASH_DELAY)

term-delay: $(TERMDELAYDEPS)
	sleep $(TERM_DELAY)
