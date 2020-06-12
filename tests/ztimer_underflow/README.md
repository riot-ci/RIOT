# Introduction

This test application underflows ztimer clocks by setting a time offset of zero.
The tests succeeds if the board running the test does not get stuck.


## ZTIMER_USEC

Uncomment the respective lines in the test's `Makefile`. If the test stucks,
increase `CONFIG_ZTIMER_USEC_MIN`.


## ZTIMER_MSEC

Uncomment the respective lines in the test's `Makefile`. If the test stucks,
increase `RTT_MIN_VALUE`.
