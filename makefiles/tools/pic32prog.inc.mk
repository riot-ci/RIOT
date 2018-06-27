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
# This requires changing the firmware to 'scripting mode'
# Should be done from a Windows computer/virtual machine as described here
#
# https://github.com/RIOT-OS/RIOT/blob/master/dist/tools/pic32prog/doc.md
#
#
# Udev rule
# ---------
#
# Add yourself to the `plugdev` group, add the following `udev` rule to
# `/etc/udev/rules.d/26-microchip.rules` and reboot.
#
# ```
# # Adapted from http://en.microstickplus.com/mplabx-on-linux
# ATTR{idVendor}=="04d8", MODE="664", GROUP="plugdev"
# ```

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
