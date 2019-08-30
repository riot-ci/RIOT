APPLICATION = tls-wolfssl

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# lwIP's memory management doesn't seem to work on non 32-bit platforms at the
# moment.
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano \
                   arduino-uno chronos esp8266-esp-12x esp8266-olimex-mod \
                   esp8266-sparkfun-thing jiminy-mega256rfr2 mega-xplained \
                   msb-430 msb-430h telosb waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 z1
BOARD_INSUFFICIENT_MEMORY = blackpill bluepill i-nucleo-lrwan1 \
                            nucleo-f031k6 nucleo-f042k6 \
                            nucleo-l031k6 nucleo-f030r8 nucleo-f302r8 \
                            nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                            saml10-xpro saml11-xpro stm32f0discovery

LWIP_IPV4 ?= 0
CFLAGS += -DTHREAD_STACKSIZE_MAIN=20000

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  LWIP_IPV6 ?= 0
else
  LWIP_IPV6 ?= 1
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
  USEMODULE += lwip_tcp lwip_sock_tcp
endif

USEMODULE += lwip_netdev
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += netdev_default

USEMODULE += inet_csum
USEMODULE += lwip_netdev
USEMODULE += lwip_sock_tcp
USEMODULE += netdev_eth
USEMODULE += ps

DISABLE_MODULE += auto_init
ifeq ($(BOARD),native)
  USEMODULE += lwip_ethernet
endif

CFLAGS += -DSO_REUSE
CFLAGS += -DLWIP_SO_RCVTIMEO
CFLAGS += -DLWIP_SOCK_TCP_ACCEPT_TIMEOUT=500
CFLAGS += -DLWIP_NETIF_LOOPBACK=1
CFLAGS += -DLWIP_HAVE_LOOPIF=1

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands

CFLAGS+=-Wno-unused-parameter -Wno-unused-variable

# Uncomment next line to enable debug symbols
#CFLAGS+=-g -ggdb3

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# need these for seeding wolfCrypt DRBG
USEMODULE += random
USEMODULE += prng_minstd

USEPKG += wolfssl
USEMODULE += wolfssl_socket wolfcrypt_ecc wolfssl_dh
USEMODULE += wolfcrypt_poly1305 wolfcrypt_chacha


include $(RIOTBASE)/Makefile.include
