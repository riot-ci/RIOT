# Compute CPU_LINE
LINE := $(shell echo $(CPU_MODEL) | tr 'a-z-' 'A-Z_' | sed -E -e 's/^STM32F0([0-9][0-9])(.)(.)/\1 \2 \3/')
TYPE := $(word 1, $(LINE))
MODEL1 := $(word 2, $(LINE))
MODEL2 := $(word 3, $(LINE))

ifneq (, $(filter $(TYPE), 30 70))
    CPU_LINE = CPU_LINE_STM32F0$(TYPE)X$(MODEL2)
else
    CPU_LINE = CPU_LINE_STM32F0$(TYPE)
endif
