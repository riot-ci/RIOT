# Introduction

This application tests the high abstraction level ztimer clocks usec, msec and sec
by locking three mutexes and waiting for them to
be unlocked by ZTIMER_USEC, ZTIMER_MSEC and ZTIMER_SEC
The tests succeeds if the board running the test does not get stuck.

ZTIMER_MSEC and ZTIMER_SEC will be configured following the rules described
in the ztimer documentation. Defining LOG_LEVEL to LOG_INFO some timing information
for human analysis is provided, without that the application is setup for automatic
testing (not providing timing information -since they are system and runtime dependant)
