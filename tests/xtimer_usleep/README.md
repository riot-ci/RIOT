# xtimer_usleep test application

This test tests `xtimer_usleep()` both against the timings of `xtimer_now_usec()`
 and by providing capabilities to compare against an external timer.

The sleep times can be probed at a pin if SLEEP_PIN` is set to 1 and the respective
gpio pin is define as `SLEEP_GPIO_PIN`.

## Usage
```
make flash test
```
