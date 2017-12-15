include ../Makefile.tests_common

USEPKG += tinycrypt

# The imported `test_utils.h` does throw warnings when used with `c99` on
# `native, so we build this test without `-Wextra` on native
ifeq (native,$(BOARD))
  WERROR = 0
endif

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
