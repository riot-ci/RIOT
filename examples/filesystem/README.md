# File system usage example

This basic example shows how to use a file system with RIOT in your embedded
application.

In particular, this example shows:

- how to mount/format/unmount a file system, with spiffs, littlefs and constfs
  examples
- how to open/read/write/close a file with and without newlib

In RIOT, most filesystems use `mtd` as flash interface. So to use this example
one must define `MTD_0`. `MTD_0` is a pointer to a `mtd_dev_t` instance.

This example uses `littlefs` as default file system on the whole `mtd` device.
A `constfs` file system is also demonstrated with two files.

All the RIOT file systems are used through the `vfs` interface, and on most
platforms they can be accessed transparently with `open/close/read/write/...`
functions.
With newlib, `fopen/fclose/fread/fwrite/...` can also be used transparently.

The following commands are available:

- `format`: should be called the first time only, it will format the `mtd`
  device with the configured file system
- `mount`: mount the file system on the configured mount point (default is
  `/sda` but it can be configured with `FLASH_MOUNT_POINT` define). The
  `constfs` file system is mounted automatically when the application starts
- `umount`: unmount `/sda`
- `cat <file>`: similarly to unix `cat` unix command, it prints the given
  `<file>` on stdout
- `tee <file> <str>`: similarly to `tee` unix command, it writes `<str>` in
  `<file>`

Apart from these commands, the default `vfs` commands can be used, for
instance:

- `vfs df`: shows all mountpoints and used/available file system space
- `vfs ls <path>`: list files
