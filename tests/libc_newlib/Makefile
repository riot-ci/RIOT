include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += all
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include

# Compile time tests
.PHONY: compile-test test-newlib-nano
all: compile-test

ifneq (,$(filter newlib,$(USEMODULE)))
  COMPILE_TESTS += test-newlib
  ifneq (,$(filter newlib_nano,$(USEMODULE)))
    CMP_OP = =
  else
    CMP_OP = !=
  endif
endif

test-newlib: $(ELFFILE)
	$(Q)\
	PRINTF_ADDR=$$($(NM) $^ | sed -n '/ printf$$/ s/ .*//p');\
	IPRINTF_ADDR=$$($(NM) $^ | sed -n '/ iprintf$$/ s/ .*//p');\
	echo "Test: comparing addresses of 'printf' and 'iprintf' symbols";\
	if test "$${PRINTF_ADDR}" $(CMP_OP) "$${IPRINTF_ADDR}" ; then \
	  echo "[SUCCESS] '$${PRINTF_ADDR}' $(CMP_OP) '$${IPRINTF_ADDR}' is True"; \
	else \
	  echo "[FAILED] '$${PRINTF_ADDR}' $(CMP_OP) '$${IPRINTF_ADDR}' is False"; \
	  exit 1; \
	fi

compile-test: $(COMPILE_TESTS)


test:
	tests/01-run.py
