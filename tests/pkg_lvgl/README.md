LittlevGL sample application
============================

This applications a basic usage of LittlevGL with RIOT: it's an adaption of one
of the upstream examples, the
[sysmon example](https://github.com/littlevgl/lv_examples/tree/master/lv_apps/sysmon).

### Flashing the application

The application works without modifications on the stm32f429i-disc1 board. To
build, flash and run the application for this board, just use:

```
make BOARD=stm32f429i-disc1 -C tests/pkg_lvgl flash
```

### Expected result

The application should display a basic system monitor with the CPU and memory
usage of the board.
Note that on the pintime, due to a too small resolution, some information are
cropped.
