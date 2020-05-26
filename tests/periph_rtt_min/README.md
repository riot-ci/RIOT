## About

This applications is meant to determine RTT_MIN_OFFSET for a specific BOARD.

## Usage

Run `BOARD=<board> make -C tests/periph_rtt_min/ flash test` the value will
be printed as:

```
Evaluate RTT_MIN_OFFSET over 1024 samples
........................................................................
........................................................................
........................................................................
........................................................................
........................................................................
RTT_MIN_OFFSET for <board>: 2
```
