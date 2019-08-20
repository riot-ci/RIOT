# name of your application
APPLICATION = default

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#USEMODULE += schedstatistics

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include and auto-initialize all available sensors
USEMODULE += saul_default

# List of boards with a network interface
BOARD_PROVIDES_NETIF += acd52832
BOARD_PROVIDES_NETIF += airfy-beacon
BOARD_PROVIDES_NETIF += b-l072z-lrwan1
BOARD_PROVIDES_NETIF += cc2538dk
BOARD_PROVIDES_NETIF += fox
BOARD_PROVIDES_NETIF += iotlab-a8-m3
BOARD_PROVIDES_NETIF += iotlab-m3
BOARD_PROVIDES_NETIF += lobaro-lorabox
BOARD_PROVIDES_NETIF += lsn50
BOARD_PROVIDES_NETIF += microbit
BOARD_PROVIDES_NETIF += msba2
BOARD_PROVIDES_NETIF += mulle
BOARD_PROVIDES_NETIF += native
BOARD_PROVIDES_NETIF += nrf51dk
BOARD_PROVIDES_NETIF += nrf51dongle
BOARD_PROVIDES_NETIF += nrf52840-mdk
BOARD_PROVIDES_NETIF += nrf52840dk
BOARD_PROVIDES_NETIF += nrf52dk
BOARD_PROVIDES_NETIF += nrf6310
BOARD_PROVIDES_NETIF += nucleo-f767zi
BOARD_PROVIDES_NETIF += openmote-cc2538
BOARD_PROVIDES_NETIF += pba-d-01-kw2x
BOARD_PROVIDES_NETIF += remote-pa
BOARD_PROVIDES_NETIF += remote-reva
BOARD_PROVIDES_NETIF += samr21-xpro
BOARD_PROVIDES_NETIF += spark-core
BOARD_PROVIDES_NETIF += telosb
BOARD_PROVIDES_NETIF += yunjia-nrf51822
BOARD_PROVIDES_NETIF += z1

ifneq (,$(filter $(BOARD),$(BOARD_PROVIDES_NETIF)))
  # Use modules for networking
  # gnrc is a meta module including all required, basic gnrc networking modules
  USEMODULE += gnrc
  # use the default network interface for the board
  USEMODULE += gnrc_netdev_default
  # automatically initialize the network interface
  USEMODULE += auto_init_gnrc_netif
  # shell command to send L2 packets with a simple string
  USEMODULE += gnrc_txtsnd
  # the application dumps received packets to stdout
  USEMODULE += gnrc_pktdump

  # We use only the lower layers of the GNRC network stack, hence, we can
  # reduce the size of the packet buffer a bit
  CFLAGS += -DGNRC_PKTBUF_SIZE=512
endif

FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msba2,$(BOARD)))
  USEMODULE += mci
  USEMODULE += random
endif

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
  DEFAULT_CHANNEL ?= 5
  CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
else                                          # radio is IEEE 802.15.4 2.4 GHz
  DEFAULT_CHANNEL ?= 26
  CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
endif
