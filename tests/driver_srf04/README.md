tests/driver_srf04
================
This example shows the usage of an srf04 module.
The application exerts a timer and two gpio.
The module is an ultrasonic range finder with a small protocoll,
hence an implementation can be kept simple.

Usage
=====

Build, flash and start the application:
```
export BOARD=your_board
make -j 2 flash term
