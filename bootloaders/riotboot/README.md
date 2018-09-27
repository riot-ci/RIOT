# Overview
This folder contains a simple bootloader called "riotboot".
A header with metadata, of length `RIOT_HDR_LEN` precedes
the actual image. The header contains "RIOT" as a magic
number to recognise a RIOT firmware image, a checksum, and
the version of the RIOT firmware `APP_VER`.
This bootloader verifies the checksum of the image which is located
at an offset (`ROM_OFFSET`) with respect to  the `ROM_START_ADDR`
defined by the CPU, just after the space allocated for riotboot.

# Usage
Just compile your application using the target `riotboot`. The header
is generated automatically according to your `APP_VER`, which can be
optionally set (0 by default) in your makefile.

## Flashing
The image can be flashed using `riotboot/flash` which also flashes
the bootloader.

e.g. `BOARD=samr21-xpro APP_VER=$(date +%s) make -C examples/hello-world riotboot/flash`

The command compiles both the hello-world example and riotboot,
generates the header and attaches it at the beginning of the example
binary.

A comprehensive test is available at tests/riotboot.
