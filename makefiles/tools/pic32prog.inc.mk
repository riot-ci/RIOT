# pic32prog flasher
# =================
#
# https://github.com/sergev/pic32prog
#
# Allow flashing pic32 boards using:
#  * Microchip PICkit2
#  * Microchip PICkit3 with script firmware
#
#
# PICkit-3
# --------
#
# Changing the firmware to 'scripting mode'
# Should be done from a Windows computer as described here
#
# http://ww1.microchip.com/downloads/en/DeviceDoc/PICkit3%20Programmer%20Application%20v3.10.zip
#
# ### Setup summary
#
# * Extract PICkit3 Programmer Application Setup v3.10.zip
# * setup.exe
# * Run PICkit3
# * Tools/Download PICkit Operating System
#   * Select C://Program Files/Microchip/PICkit3/PK3OSV020005.hex
#

PIC32PROG = $(RIOTTOOLS)/pic32prog/pic32prog

FLASHFILE ?= $(HEXFILE)

FLASHER ?= $(PIC32PROG)
FFLAGS  ?= $(FLASHFILE)

# No reset command
RESET ?=
RESET_FLAGS ?=

# Compile pic32prog if using the one provided in RIOT
ifeq ($(PIC32PROG),$(FLASHER))
  FLASHDEPS += $(PIC32PROG)
endif
