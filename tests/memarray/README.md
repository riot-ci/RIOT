Expected result
===============

This application should run a number of tests equal to NUMBER_OF_TESTS (Default 12).

At the beginning of the tests is printed the memory being used by the threads.
At the end of the tests, the threads memory usage is printed once more time.

This test is passed if the memory used by the main thread remains static.

Background
==========

The module `memarray` is the static memory allocator for RIOT-OS

This test application is therefore specialized for only testing the use of the module.

This is also inspired by `test/malloc`
