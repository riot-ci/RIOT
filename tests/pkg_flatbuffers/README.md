FlatBuffer sample application
=============================

This application shows how to the FlatBuffer library from Google for
serializing/deserializing a runtime C++ object to/from a binary buffer.

The application is
[taken as is from the upstream repository](https://github.com/google/flatbuffers/blob/master/samples/sample_binary.cpp).

More details are available on
[the FlatBuffers website](http://google.github.io/flatbuffers/).

Usage
-----

Before building the application, ensure the `flatc` compiler is already built:

    make -C dist/tools/flatc

Then, simply run the application on the board of your choice using:

    make BOARD=<board of your choice> -C tests/pkg_flatbuffers flash term

Expected result
---------------

The application should simply display:

    "The FlatBuffer was successfully created and verified!"
