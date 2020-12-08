Emulators                                                 {#emulators}
=========

RIOT supports the [Qemu](https://www.qemu.org/) and [Renode](https://renode.io/)
emulation tools. The emulation offers a hardware-free devleopment environment
for quickly testing applications.

From the build system point of view, the emulation support in RIOT is
transparent compared to the usual workflow: simply add `EMULATE=1` to the
command line and the emulator supported by the board will be called instead of
the board flashing/debugging tool.
Targets compatible with `EMULATE=1` are `term`, `cleanterm`, `debug`,
`debugserver` and `test`.

If a board support multiple emulators, the emulator backend can be selected
with the `RIOT_EMULATOR` variable. Possible values are `qemu` and `renode`.

## Features

Be aware that not all hardware features provided by a board - and described as
such in the build system - are implemented in emulators. For example, the
`hifive1b` provides the RTC peripheral feature but this is not implemented by
the renode driver.
So you may expect some failures when running advanced applications on the
emulator.

## Usage

All emulators can be used the same way. Just add `EMULATE=1` to the command
line.

To start an emulator and connect to the serial port of the emulated board, run:

```
$ EMULATE=1 make BOARD=<board> -C <application directory> all term
```

To start an emulator with a GDB server and connect a GDB client to it, run:

```
$ EMULATE=1 make BOARD=<board> -C <application directory> all debug
```

To start an automatic test script with the emulated board, run:

```
$ EMULATE=1 make BOARD=<board> -C <test application directory> all test
```

# Qemu

## Overview

[Qemu](https://www.qemu.org/) is a machine emulator and virtualizer. It can
be used to emulate regular computer architectures but also some microcontroller
based boards such as the @ref boards_microbit.

## Installation

Qemu is usually available via the package manager of common Linux distributions.
Depending on you local systemn installation procedure is described on the
[qemu website](https://www.qemu.org/download/).

## Boards supported

So far, only the @ref boards_microbit board is supported with qemu.

# Renode

## Overview

[Renode](http://renode.io) is a virtual development tool for multinode embedded
networks (both wired and wireless) enabling a scalable workflow for building
effective, tested and secure IoT systems, created by
Antmicro](http://antmicro.com/blog/2017/08/renode-press-release/).
It can easily be used to run applications on a broad range of embedded platforms
without any changes in the code itself, as if you were running on real
hardware - but with more possibilities.

## Installation

### From package

Packages for macOS, deb-based and rpm-based systems, for Windows and for Arch
Linux are available on [GitHub](https://github.com/renode/renode/releases/latest).

### From source

Follow the installation instructions on Renode's
[GitHub](https://github.com/renode/renode#installation) page.

After compilation is successful, ensure that `renode` is available on your
`PATH`.
One way to do so, is via symlink:

```
sudo ln -s path/to/renode/repository/renode /usr/local/bin/renode
```

### Testing

After installation, verify if Renode is working using `renode --help`. You
should be presented with a help screen.

## Documentation

Documentation for Renode can be found on [Read The Docs](https://renode.readthedocs.io).

## Usage

From within RIOT-OS, add `EMULATE=1` to start emulation. The emulation expects
a board definition file in `boards/<BOARD>/dist/board.resc`.

The board definition file will tell Renode how to setup an emulation session.
The application binary file (`*.elf`) is available using the variable
`$image_file`.

For an example, refer to `boards/cc2538dk/dist/board.resc`.

The renode logging level can be configured from the command line using the
following variables:
- `RENODE_SHOW_LOG`: set it to 1 to show the logs in the standard output
- `RENODE_LOG_LEVEL`: set it to the desired log level, default is 2 (warning)

The renode monitor and serial console GUI windows are hidden by default but
they can be displayed by setting `RENODE_SHOW_GUI` to 1 in the command line.
