RIOTBASE       := ../../..
RIOT_INCLUDE   := $(RIOTBASE)/sys/include
COMMON_SRC     := common.c
COMMON_HDR     := common.h

RIOT_HDR_SRC := \
	$(RIOTBASE)/sys/checksum/fletcher32.c \
	$(RIOTBASE)/sys/riot_hdr/riot_hdr.c

RIOT_HDR_HDR := $(RIOT_INCLUDE)/riot_hdr.h \
	$(RIOT_INCLUDE)/checksum/fletcher32.h

GENHDR_SRC := $(COMMON_SRC) $(RIOT_HDR_SRC) \
	main.c genhdr.c

GENHDR_HDR := $(COMMON_HDR) $(RIOT_HDR_HDR)

CFLAGS += -g -I. -O3 -Wall -Wextra -pedantic -std=c99

all: bin/genhdr

bin/:
	mkdir -p bin

bin/genhdr: $(GENHDR_HDR) $(GENHDR_SRC) Makefile | bin/
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) $(GENHDR_SRC) -o $@

clean::
	rm -rf bin/genhdr
