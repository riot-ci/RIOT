Running and creating tests
=================

There are a number of tests included in RIOT.  They are located in the [test folder](https://github.com/RIOT-OS/RIOT/tree/master/tests).  These tests allow basic functionality to be verified as well as provide an example of usage.


Running tests with testrunner
=================
Some tests can be preformed automatically.  The tests are defined in the tests/<test_application>/tests/ folder.  They are written in python and interacted with the test application code, usually doing some validation.  It is recommended to flash the board with the test before running it.

To run tests call:

`BOARD=<board_of_your_choice> make -C tests/<test_application>/ flash test`
