include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-duemilanove arduino-leonardo \
                             arduino-mega2560 arduino-nano arduino-uno \
                             b-l072z-lrwan1 blackpill-128kib blackpill \
                             bluepill-128kib bluepill calliope-mini \
                             cc2650-launchpad cc2650stk hifive1 hifive1b \
                             i-nucleo-lrwan1 lsn50 maple-mini mega-xplained \
                             microbit msb-430 msb-430h nrf51dongle nrf6310 \
                             nucleo-f042k6 nucleo-f031k6 nucleo-f030r8
                             nucleo-f070rb nucleo-f072rb nucleo-f103rb
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8
                             nucleo-l031k6 nucleo-l073rz opencm904 \
                             pic32-clicker pic32-wifire saml10-xpro \
                             saml11-xpro spark-core stm32l0538-disco \
                             stm32f0discovery thingy52 telosb waspmote-pro
                             wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1
# chronos, hamilton mips-malta, and ruuvitag boards don't support ethos
BOARD_BLACKLIST := chronos hamilton mips-malta ruuvitag

export TAP ?= tap0

USEMODULE += emcute
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += sock_util
USEMODULE += auto_init_gnrc_netif
# use Ethernet as link-layer protocol
ifeq (native,$(BOARD))
  USEMODULE += netdev_tap

  TERMFLAGS ?= $(TAP)
else
  USEMODULE += stdio_ethos

  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

CFLAGS += -DEMCUTE_TOPIC_MAXLEN="249"   # 256 - 7
CFLAGS += -DSTDIO_UART_RX_BUFSIZE="512" # Adapt to SHELL_BUFSIZE in app

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
