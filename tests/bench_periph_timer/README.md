# Benchmark test for periph_timer

This test is intended to collect statistics about the runtime delays in the
periph_timer implementation. This tool is mainly intended to detect problems in
the low level driver implementation which can be difficult to detect from
higher level systems such as xtimer.

## Test outline

The test consists of a single thread of execution which will set random
timeouts on a periph_timer device and measure the time it takes until the
callback is called. The difference between the expected and the measured time
is computed and the mean and variance of the recorded values is calculated. The
results are printed as a table on stdout every 15-30 seconds. Both timer_set
and timer_set_absolute calls are mixed in a random order. If the mean or
variance of the difference is exceptionally large, the row will be marked with
"SIC!" to draw attention to the fact.

## Configuration

The timer under test is compared against a reference timer, these can be chosen
at compile time by defining TIM_TEST_DEV and TIM_REF_DEV. The frequencies for
these timers can be configured through TIM_DEV_FREQ and TIM_REF_FREQ. For
example, to compare timer device 2 running at 32768 Hz against timer device 0
running at 1 MHz on Mulle:

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
variance will also be greater because of the quantization errors and rounding in
the tick conversion routine.

### Rule of thumb

Below are some rules of thumb that can be useful when looking at the results.
These are only guidelines for when debugging timer drivers, the real world
requirements vary from application to application.

1 MHz timer tested with 1 MHz reference:

abs(mean) < 10
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
    === timer_set ===
    interval   count       sum       sum_sq    min   max  mean  variance
          1:    1149     54732      2607398     47    49    47     60
          2:    1162     54855      2589767     46    49    47     19
          3:    1150     54926      2623586     47    49    47     72
          4:    1144     54076      2556382     46    52    47     25
          5:    1128     53805      2566707     47    48    47     66
          6:    1146     54056      2549962     46    48    47     16
          7:    1155     55104      2629410     47    62    47     67
          8:    1168     55110      2600470     46    48    47     17
...

The statistics above show that the timer implementation introduces a small
delay on all timers. Note however that it is not clear whether this delay is in
timer_set, timer_read, or within the testing code, but the combined effect of
all of those error sources make the timer overshoot its target by on average 47
microseconds.

Below is a short sample of a test where there is something wrong with the timer
implementation, a 32768 Hz timer tested with a 1 MHz reference:

...
          14:    1212     57194      2699172     46    48    47     18
          15:    1180     56273      2683855     47    48    47     65
          16:    1116     55573     11337871     46  2976    49   7765  <=== SIC!
          17:    1107     52790      2517664     47    49    47     65
          18:    1162     54839      2588245     46    48    47     18
          19:    1177     56111      2675233     47    48    47     63
          20:    1060     50021      2360653     46    48    47     18
          21:    1142     54431      2594601     47    49    47     63
          22:    1090     51422      2426064     46    48    47     16
          23:    1108     52806      2516932     47    49    47     62
          24:    1124     53056      2507258     46    99    47     21
          25:    1137     54219      2585735     47    49    47     65
          26:    1159     54673      2579249     46    48    47     16
          27:    1103     52589      2507591     47    49    47     64
          28:    1064     49313      3038659   -821    48    46    740  <=== SIC!
          29:    1122     52347      2442511     46    48    46     60
          30:    1197     56474      2664626     46    48    47     17
          31:    1142     53292      2487152     46    47    46     61
          32:    1142     53893      2543503     46    48    47     18
          33:    1156     53973      2520229     46    48    46     64
          34:    1123     52998      2501358     46    48    47     18
          35:    1109     51768      2416772     46    48    46     63
          36:    1158     54591      2573735     46    48    47     13
          37:    1108     51677      2410467     46    48    46     59
          38:    1125     54073      3597957     46  1047    48    894  <=== SIC!
          39:    1114     51983      2425955     46    48    46     61
          40:    1156     65059    114674845     46 10588    56  96146  <=== SIC!
          41:    1131     52785      2463787     46    48    46     62
...
