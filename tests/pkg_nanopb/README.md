# Introduction

This is a test application for the nanoPb library.
The library provides a Google Protocol Buffers encoder / decoder.

# Prerequisites

Install the protobuf compiler and the protobuf python bindings.
On Debian/ubuntu, the corresponding packages are `protobuf-compiler` and
`python-protobuf`. On Arch, it is `protobuf` and `python-protobuf`.

You'll also need the nanopb protoc generator plugin.

Checkout the matching nanopb repository:

    $ git clone https://github.com/nanopb/nanopb nanopb-0.3.9.3
    $ cd nanopb/generator/proto

Take note of the path to `nanopb/generator`.

The build system needs to know the location of the generator plugin.
Assuming nanopb has been checked out into `~/src/nanopb`, you'll need to prefix
all make invocations with
```PROTOC_GEN_NANOPB=~/src/nanopb/generator/protoc-gen-nanopb```.

E.g., to compile this test:

    PROTOC_GEN_NANOPB=~/src/nanopb/generator/protoc-gen-nanopb make \
        clean all -j4
