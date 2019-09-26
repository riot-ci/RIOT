# name of your application
APPLICATION = lorawan

# Use the ST B-L072Z-LRWAN1 board by default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

# For boards providing an EEPROM (e.g. STM32 L0 and L1 for the moment), the
# lowest possible power mode is enabled (assuming no RAM retention is available
# in this mode, this is the case for STANDBY mode on STM32).
BOARD_PROVIDING_EEPROM := b-l072z-lrwan1 \
                          lobaro-lorabox \
                          lsn50 \
                          i-nucleo-lrwan1 \
                          nucleo-l073rz \
                          nucleo-l053r8 \
                          nucleo-l152re \
                          nz32-sc151 \
                          #

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

# Send a message every 20s after joining the network
SEND_PERIOD ?= 20

# Default radio driver is Semtech SX1276 (used by the B-L072Z-LRWAN1 board)
DRIVER ?= sx1276

# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# Include the Semtech-loramac package
USEPKG += semtech-loramac

USEMODULE += $(DRIVER)
USEMODULE += fmt
FEATURES_REQUIRED += periph_rtc

CFLAGS += -DREGION_$(REGION)
CFLAGS += -DSEND_PERIOD=$(SEND_PERIOD)
ifeq (otaa,$(ACTIVATION_MODE))
  CFLAGS += -DDEVEUI=\"$(DEVEUI)\" -DAPPEUI=\"$(APPEUI)\" -DAPPKEY=\"$(APPKEY)\"
  CFLAGS += -DUSE_OTAA
else ifeq (abp,$(ACTIVATION_MODE))
  CFLAGS += -DDEVADDR=\"$(DEVADDR)\" -DNWKSKEY=\"$(NWKSKEY)\" -DAPPSKEY=\"$(APPSKEY)\"
  CFLAGS += -DRX2_FREQ=$(RX2_FREQ) -DRX2_DR=$(RX2_DR)
  CFLAGS += -DUSE_ABP
endif

# Enable persistence on EEPROM and lowest possible power mode only if the board
# provides the EEPROM feature
ifneq (,$(filter $(BOARD), $(BOARD_PROVIDING_EEPROM)))
  FEATURES_REQUIRED += periph_eeprom
  # Enable lowest possible power mode (e.g. STANDBY mode on STM32) which
  # in general doesn't provide RAM retention after wake-up.
  # This low-power mode can only be used with boards providing an internal
  # EEPROM to be able to restore the state of MAC from a non-volatile memory.
  CFLAGS += '-DPM_BLOCKER_INITIAL={ .val_u32 = 0x01010100 }'
else
  # WARNING: the next line disables the dutycycle restriction from the MAC. This
  # means that the sending period must be greater than the allowed dutycycle.
  # Check your LoRaWAN regional parameters and use a valid SEND_PERIOD value.
  #
  # Note that this is used as a workaround for issues with the internal use
  # of xtimer in the semtech-loramac adaption code for RIOT. Indeed, during a
  # sleep mode with RAM retention the hardware timer used by default are stopped
  # and thus, when waking up the dutycycle timer time has not changed. This
  # prevents more than one message to be sent.
  # This is a known limitation of xtimer and should be fixed in the future by
  # using low power timer (based on 32.768kHz external oscillator).
  $(warning You are disabling dutycycle restriction, make sure your sending \
            period is greater than the allowed dutycycle in your geographic \
            region)
  CFLAGS+=-DDISABLE_LORAMAC_DUTYCYCLE
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
