# Benchmark test for periph_timer

This test is intended to collect statistics about the runtime delays in the
periph_timer implementation. This tool is mainly intended to detect problems in
the low level driver implementation which can be difficult to detect from
higher level systems such as xtimer.

## Test outline

The basic idea of the test is to generate a wide variety of calls to the
periph/timer API, in order to detect problems with the software implementation.
The test consists of a single thread of execution which will set random
timeouts on a periph_timer device. A reference timer is used to measure the
time it takes until the callback is called. The difference between the expected
time and the measured time is computed, and the mean and variance of the
recorded values are calculated. The results are printed as a table on stdout
every 20-90 seconds. All of the test scenarios used in this application are
based on experience from real world bugs encountered during the development of
RIOT and other systems.

### Avoiding phase lock

The CPU time used for generation and setting of the next timer target in
software is approximately constant across iterations. When the application flow
is driven by the timer under test, via a mutex which is unlocked from the timer
callback, the application will be "phase locked" to the timer under test. This
peculiarity may hide certain implementation race conditions which only occur at
specific phases of the timer ticks. In the test application, this problem is
avoided by inserting random CPU delay loops at key locations:

 - After timer_stop
 - Before timer_set/timer_set_absolute
 - Before timer_start

### API functions tested

Both timer_set and timer_set_absolute calls are mixed in a random order, to
ensure that both functions are working correctly.

### Rescheduling

Some bugs may occur only when the application attempts to replace an already
set timer by calling timer_set again while the timer is running. This is dubbed
"resched" in this application. The application will issue rescheduling timer
calls in the normal operation of the test to catch this class of problems.

### Setting stopped timers

Another class of bugs is the kind where a timer is not behaving correctly if it
was stopped before setting a timer target. The timer should set the new target,
but not begin counting towards the target until timer_start is called. This is
covered by this application under the "stopped" category of test inputs.

## Result presentation

When the test has run for a certain amount of time, the current results will be
presented as a table on stdout. To assist with finding the source of any
discrepancies, the results are split according to three parameters:

 - Function used: timer_set, timer_set_absolute
 - Rescheduled: yes/no
 - Stopped: yes/no

## Configuration

The timer under test and the reference timer can be chosen at compile time by
defining TIM_TEST_DEV and TIM_REF_DEV, respectively. The frequencies for
these timers can be configured through TIM_DEV_FREQ and TIM_REF_FREQ. For
example, to compare timer device 2 running at 32768 Hz against a reference timer
on timer device 0 (default) running at 1 MHz (default) on Mulle:

    CFLAGS='-DTIM_TEST_DEV=TIMER_DEV\(2\) -DTIM_TEST_FREQ=32768' BOARD=mulle make flash

### Default configuration

reference timer: TIMER_DEV(0)
timer under test: TIMER_DEV(1) if TIMER_NUMOF > 1, TIMER_DEV(0) otherwise.
timer frequency: 1 MHz

Define USE_REFERENCE=0 to compare a timer against itself. Be aware that this
may hide a class of errors where the timer is losing ticks in certain
situations, which a separate reference timer would be able to detect.

## Expected result

It is difficult to set a general expected result which applies to all
platforms. However, the mean and variance of the differences should be small,
in relation to the tick duration of the timer under test. What is "small" also
depends the CPU core frequency, because of CPU processing time spent in the
driver code and in the test code. For example, when testing a 1 MHz timer and
comparing with another 1 MHz timer, the mean difference should likely be in
single digits on a Cortex-M CPU. Testing a 32768 Hz timer and referencing a 1
MHz timer on the other hand should have a mean difference in double digits, the
variance will also be greater because of the quantization errors and rounding
in the tick conversion routine. If the mean or variance of the difference is
exceptionally large, the row will be marked with "SIC!" to draw attention to
the fact.

### Rule of thumb

Below are some rules of thumb that can be useful when looking at the results.
These are only guidelines for when debugging timer drivers, the real world
requirements vary from application to application. The CPU core clock speed will
also affect the measured mean difference because of the execution time of the
code between the timer interrupt being triggered and when the benchmark
application has read out both timer values. The variance should not be affected
by the CPU speed because the processing time between the ISR and the timer_read
should be constant across all iterations.

1 MHz timer tested with 1 MHz reference:

abs(mean) < 20
variance < 10

32768 Hz timer tested with 1 MHz reference:

abs(mean) < 100
variance < 100

32768 Hz timer tested with 32768 Hz reference:

abs(mean) < 4
variance < 10

1 MHz timer tested with 32768 Hz reference:

abs(mean) < 2
variance < 10

### Example output

Below is a short sample of a test of a 32768 Hz timer with a 1 MHz reference:

    ------------- BEGIN STATISTICS --------------
    Target error (actual trigger time - expected trigger time)
    positive: timer is late, negative: timer is early
    === timer_set running ===
       interval    count       sum       sum_sq    min   max  mean  variance
       1 -    2:     502     28976        38942     43    74    57     77
       3 -    4:     423     24826        31372     43    74    58     74
       5 -    8:     887     52075        65411     43    74    58     73
       9 -   16:    1809    105768       136714     43    74    58     75
      17 -   32:    3671    214870       281126     42    74    58     76
          TOTAL     7292    426515       554067     42    74    58     75
...

The statistics above show that the timer implementation introduces a small
delay on all timers. Note however that it is not clear whether this delay is in
timer_set, timer_read, or within the testing code, but the combined effect of
all of those error sources make the timer overshoot its target by on average 58
microseconds. There is also the expected difference of approximately 31 µs
between the minimum and the maximum values, this is entirely expected because
one 32768 Hz tick equals 1/32768 s = 30.51 µs.

Below is a short sample of a test where there is something wrong with the timer
implementation, again a 32768 Hz timer tested with a 1 MHz reference:

    === timer_set_absolute running ===
       interval    count       sum       sum_sq    min   max  mean  variance
       1 -    2:     706    393603     59023835     44  1075   557  83717  <=== SIC!
       3 -    4:     644    358001     56925795     43  1074   555  88495  <=== SIC!
       5 -    8:    1402    783661    121240925     45  1074   558  86534  <=== SIC!
       9 -   16:    2762   1515993    243789723     44  1073   548  88288  <=== SIC!
      17 -   32:    5476   2981671    490016153     43  1075   544  89500  <=== SIC!
          TOTAL    10990   6032929    971284075     43  1075   548  88386  <=== SIC!

We can see that the variance, the maximum error, and the mean are very large.
This particular timer implementation needs some work on its timer_set_absolute
implementation.

## Configuration details

Configuration macros used by the application are described below

### Settings related to timer hardware

#### TIM_TEST_DEV

Timer under test. Default: `TIMER_DEV(1)` if `TIMER_NUMOF > 1`, `TIMER_DEV(0)`
otherwise

#### TIM_REF_DEV

Reference timer used for measuring the callback time error. Default: `TIMER_DEV(0)`

#### TIM_TEST_FREQ

Frequency of the timer under test, used when calling timer_init during the
application startup. Default: `1000000`, 1 MHz

#### TIM_REF_FREQ

Frequency of the reference timer, used when calling timer_init during the
application startup. Default: `1000000`, 1 MHz

#### TIM_TEST_CHAN

Timer channel used on the timer under test. Default: `0`

#### USE_REFERENCE

Shorthand for setting the reference timer to the same as the timer under test.
Default: `0`

### Settings related to result processing


#### DETAILED_STATS

Keep statistics per timer target offset value if set to 1, otherwise keep
statistics only per scenario. Default: `1`

#### LOG2_STATS

Only used when `DETAILED_STATS == 1`. Statistics are grouped according to
2-logarithms of the timer offset value, e.g. 1-2, 3-4, 5-8, 9-16 etc. This
reduces memory consumption and creates a shorter result table for easier
overview. Default: `1`

#### TEST_PRINT_INTERVAL_TICKS

The result table will be printed to standard output when the sum of the tested
timer offsets have passed this value. Default: `(TIM_TEST_FREQ * 15)`

### Settings related to timer input generation

#### TEST_MIN

Minimum timer offset tested, in timer under test ticks. Default: `1` for
timers < 1 MHz, `2` otherwise

#### TEST_MAX

Maximum timer offset tested, in timer under test ticks. Default: `128`

#### SPIN_MAX_TARGET

The CPU busy wait loop will be calibrated during application start up so that
the maximum random spin length is approximately equal to this many timer under
test ticks. Default: `16`

#### TEST_UNEXPECTED_VARIANCE

Mark the output row if the computed variance is greater than this value.
Default: `100`

#### TEST_UNEXPECTED_MEAN

Mark the output row if the computed mean absolute value is greater than this
value. Default: `100`
