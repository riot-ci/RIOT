# Description

This test verifies that the uMorse package compiles with RIOT. It also checks
its basic functions by encoding a text string into Morse code. The encoded text
is printed to the shell using `.` and `_` for DIT (short) and  DAH (long) Morse
chars. Inter char gaps are represented by ` ` (space), inter word gaps are
printed as ` / ` (dash). Even longer gaps are printed as `\n` (new line).

The Morse code will be printed 2 times using different encoding strategies
provides by uMorse. The first print out will be with delay, the second without.
However, if compiled for a board with an LED0_PIN defined, the second run will
use the LED to blink out the Morse Code.

# Configure delays

The delay can be configured by setting `UMORSE_DELAY_DIT`, see Makefile.
The default in uMorse is 60ms, as this is quiet fast for beginners, its
set to 120ms for this test and could be further raised, e.g. 240ms.
