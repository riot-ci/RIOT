# name of your application
APPLICATION = lorawan

# Use the ST B-L072Z-LRWAN1 board by default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

# Change this to abp to enable Activation By Personnalization mode
ACTIVATION_MODE ?= otaa

ifeq (otaa,$(ACTIVATION_MODE))
  DEVEUI ?= 0000000000000000
  APPEUI ?= 0000000000000000
  APPKEY ?= 00000000000000000000000000000000
else ifeq (abp,$(ACTIVATION_MODE))
  DEVADDR ?= 00000000
  NWKSKEY ?= 00000000000000000000000000000000
  APPSKEY ?= 00000000000000000000000000000000
  RX2_FREQ ?= 869525000
  RX2_DR ?= 3
else
  $(error Unsupported activation mode '$(ACTIVATION_MODE)')
endif

# Default radio driver is Semtech SX1276 (used by the B-L072Z-LRWAN1 board)
DRIVER ?= sx1276

# Default region is Europe and default band is 868MHz
REGION ?= EU868

# Include the Semtech-loramac package
USEPKG += semtech-loramac

USEMODULE += $(DRIVER)
USEMODULE += fmt
FEATURES_REQUIRED += periph_rtc
FEATURES_REQUIRED += periph_eeprom

CFLAGS += -DREGION_$(REGION)
CFLAGS += -DLORAMAC_ACTIVE_REGION=LORAMAC_REGION_$(REGION)
ifeq (otaa,$(ACTIVATION_MODE))
  CFLAGS += -DDEVEUI=\"$(DEVEUI)\" -DAPPEUI=\"$(APPEUI)\" -DAPPKEY=\"$(APPKEY)\"
  CFLAGS += -DUSE_OTAA
else ifeq (abp,$(ACTIVATION_MODE))
  CFLAGS += -DDEVADDR=\"$(DEVADDR)\" -DNWKSKEY=\"$(NWKSKEY)\" -DAPPSKEY=\"$(APPSKEY)\"
  CFLAGS += -DRX2_FREQ=$(RX2_FREQ) -DRX2_DR=$(RX2_DR)
  CFLAGS += -DUSE_ABP
  # With ABP, just keep STOP power management mode locked. STANDBY is unlocked
  # by default.
  CFLAGS += '-DPM_BLOCKER_INITIAL={ .val_u32 = 0x01010100 }'
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
