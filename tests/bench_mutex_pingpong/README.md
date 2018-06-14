# About

In this test, one thread will repeatedly lock a mutex, while another thread
will unlock it.  The result is the number of unlocks done in an interval of one
second, which amounts to half the number of incurred context switches.
