# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

include ../Makefile.tests_common

# DEVELHELP ?= 0 to reduce firmware size
DEVELHELP ?= 0

RIOT_TERMINAL ?= socat

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-uno \
                             chronos i-nucleo-lrwan1 \
                             msb-430 msb-430h nucleo-f031k6 \
                             nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                             nucleo-f302r8 nucleo-f303k8 nucleo-f334r8 \
                             nucleo-l053r8 saml10-xpro saml11-xpro \
                             stm32f0discovery telosb waspmote-pro \
                             wsn430-v1_3b wsn430-v1_4 z1

# Use different settings when compiling for one of the following (low-memory)
# boards
LOW_MEMORY_BOARDS := nucleo-f334r8

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_router_default
USEMODULE += sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# Required for nanocoap server
USEMODULE += nanocoap_sock

# include this for printing IP addresses
USEMODULE += shell
USEMODULE += shell_commands

# include riotboot modules
USEMODULE += riotboot_flashwrite
FEATURES_REQUIRED += riotboot

# Change this to 0 to not use ethos
USE_ETHOS ?= 1

ifeq (1,$(USE_ETHOS))
  GNRC_NETIF_NUMOF := 2
  USEMODULE += stdio_ethos
  USEMODULE += gnrc_uhcpc
  # ethos baudrate can be configured from make command
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)

 # make sure ethos and uhcpd are built
  TERMDEPS += host-tools

  # For local testing, run
  #
  #     $ cd dist/tools/ethos; sudo ./setup_network.sh riot0 2001:db8::0/64
  #
  #... in another shell and keep it running.
  export TAP ?= riot0
  TERMPROG = $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS = $(TAP) $(PORT)
endif

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  $(info Using low-memory configuration for microcoap_server.)
  ## low-memory tuning values
  USEMODULE += prng_minstd
endif

# Ensure both slot bin files are always generated and linked to avoid compiling
# during the test. This ensures that "BUILD_IN_DOCKER=1 make test"
# can rely on them being present without having to trigger re-compilation.
BUILD_FILES += $(SLOT_RIOT_ELFS:%.elf=%.bin)

# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves, thus add the required files here so they will be
# submitted along with the test jobs.
TEST_EXTRA_FILES += $(SLOT_RIOT_ELFS)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Currently no libcoap is available on the Pi-fleet
TEST_ON_CI_BLACKLIST = all

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  # lower pktbuf buffer size
  # Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
  ifndef CONFIG_GNRC_PKTBUF_SIZE
    CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=1000
  endif
endif

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)
