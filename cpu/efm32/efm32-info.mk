# Find the header file that should exist if the CPU is supported.
EFM32_HEADER = $(wildcard $(RIOTCPU)/efm32/families/*/include/vendor/$(CPU_MODEL).h)

ifeq (,$(EFM32_HEADER))
  $(error Header file for $(CPU_MODEL) is missing)
endif

# Lookup up CPU information using grep.
EFM32_INFO = $(shell grep $(CPU_MODEL) $(shell dirname $(EFM32_HEADER))/../../cpus.txt)

ifeq (,$(EFM32_INFO))
  $(error Unable to read CPU information for $(CPU_MODEL))
endif
