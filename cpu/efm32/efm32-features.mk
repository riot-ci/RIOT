# This file provides defaults for additional EFM32-specific features. You
# should override them from the command line, or in your Makefile. Note that
# some features may not be applicable to all EFM32 boards or CPUs.
EFM32_LEUART_ENABLED ?= 1

include $(RIOTCPU)/efm32/efm32-vars.mk

CPU_FAM      = $(word $(CPU_FAM_INDEX), $(EFM32_VARS_$(CPU_MODEL)))
CPU_SERIES   = $(word $(CPU_SERIES_INDEX), $(EFM32_VARS_$(CPU_MODEL)))
CPU_ARCH     = $(word $(CPU_ARCH_INDEX), $(EFM32_VARS_$(CPU_MODEL)))
