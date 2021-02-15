include ../Makefile.tests_common

USEMODULE += congure_mock
USEMODULE += congure_test
USEMODULE += fmt
USEMODULE += shell
USEMODULE += shell_commands

INCLUDES += -I$(CURDIR)

# Use a terminal that does not introduce extra characters into the stream.
RIOT_TERMINAL ?= socat

# Output of commands comes one command to late on this platform within the CI
# See https://github.com/RIOT-OS/RIOT/pull/16014
TEST_ON_CI_BLACKLIST += esp32-wroom-32

include $(RIOTBASE)/Makefile.include

ifndef CONFIG_SHELL_NO_ECHO
  CFLAGS += -DCONFIG_SHELL_NO_ECHO=1
endif

ifndef CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE
  CFLAGS += -DCONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE=4
  export LOST_MSG_POOL_SIZE=4
else
  export LOST_MSG_POOL_SIZE=$(CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE)
endif

test:
	pytest -v $(CURDIR)/tests/01-run.py
