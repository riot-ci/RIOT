# xtimer_usleep test application

This test tests `xtimer_usleep()` against the timings of `xtimer_now_usec()`
 and by comparing the incoming strings with the test hosts time. 

The sleep times can be probed with a oscilloscope at a pin if `SLEEP_PIN` is set
to and the respective gpio `SLEEP_PORT` is defined in the makefile.

CFLAGS += SLEEP_PIN=7
CFLAGS += SLEEP_PORT=PORTF

## Usage
Executed from the project directory
```
make BOARD=<BoardName> flash test test
```

### Expected result running 
```
018-04-03 18:00:06,825 - INFO # Connect to serial port /dev/ttyACM0
Welcome to pyterm!
Type '/exit' to exit.
2018-04-03 18:00:07,828 - INFO # n: XXX
2018-04-03 18:00:07,829 - INFO # Running test 5 times with 

2018-04-03 18:00:07,830 - INFO # main(): This is RIOT! (Version: XXX )
2018-04-03 18:00:07,830 - INFO # Running test 5 times with 3 distinct sleep times
2018-04-03 18:00:07,831 - INFO # Please hit any key and then ENTER to continue
a
a
2018-04-03 18:00:09,429 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 18:00:09,489 - INFO # Slept for 50224 us (expected: 50000 us)
2018-04-03 18:00:09,600 - INFO # Slept for 100224 us (expected: 100000 us)
2018-04-03 18:00:09,620 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 18:00:09,680 - INFO # Slept for 50224 us (expected: 50000 us)
2018-04-03 18:00:09,791 - INFO # Slept for 100224 us (expected: 100000 us)
2018-04-03 18:00:09,811 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 18:00:09,872 - INFO # Slept for 50224 us (expected: 50000 us)
2018-04-03 18:00:09,982 - INFO # Slept for 100224 us (expected: 100000 us)
2018-04-03 18:00:10,003 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 18:00:10,063 - INFO # Slept for 50224 us (expected: 50000 us)
2018-04-03 18:00:10,173 - INFO # Slept for 100224 us (expected: 100000 us)
2018-04-03 18:00:10,194 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 18:00:10,254 - INFO # Slept for 50224 us (expected: 50000 us)
2018-04-03 18:00:10,365 - INFO # Slept for 100224 us (expected: 100000 us)
2018-04-03 18:00:10,371 - INFO # Test ran for 955136 us

```

### On Error with pyterm
```
2018-04-03 18:11:48,952 - INFO # Connect to serial port /dev/ttyACM0
Welcome to pyterm!
Type '/exit' to exit.
2018-04-03 18:11:49,957 - INFO # (Version: XXX )
2018-04-03 19:12:20,747 - INFO # Running test 5 times with 7 distinct sleep times
2018-04-03 19:12:20,747 - INFO # Please hit any key and then ENTER to continue
2018-04-03 19:12:20,747 - INFO # 
a
a
2018-04-03 19:12:22,411 - INFO # 
2018-04-03 19:12:22,421 - INFO # Slept for 10224 us (expected: 10000 us)
2018-04-03 19:12:22,427 - INFO # MCU max error   70
2018-04-03 19:12:22,480 - INFO # error          224
2018-04-03 19:12:22,490 - INFO # Slept for 50232 us (expected: 50000 us)
2018-04-03 19:12:22,495 - INFO # MCU max error  70
2018-04-03 19:12:22,500 - INFO # error         232
2018-04-03 19:12:22,510 - INFO # Slept for 1456 us (expected: 1234 us)
Invalid timeout 1456 ,expected 1172 < 1234 < 1295
HOST max error  61
error           283
```
### On Error with terminal
```

```
