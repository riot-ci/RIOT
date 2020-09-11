# Copyright (c) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

config CPU_FAM_G4
    bool
    select CPU_STM32
    select CPU_CORE_CORTEX_M4
    select HAS_CPU_STM32G4
    select HAS_CORTEXM_MPU
    select HAS_PERIPH_FLASHPAGE
    select HAS_PERIPH_FLASHPAGE_RAW
    select HAS_PERIPH_HWRNG

# CPU lines
config CPU_LINE_STM32G431XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G441X
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G471XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G473XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G474XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G483XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32G484XX
    bool
    select CPU_FAM_G4

config CPU_LINE_STM32GBK1CB
    bool
    select CPU_FAM_G4

# CPU models
config CPU_MODEL_STM32G474RE
    bool
    select CPU_LINE_STM32G474XX
