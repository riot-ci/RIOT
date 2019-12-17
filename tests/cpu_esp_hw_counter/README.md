# ESP32 Periph Timer Test Using Hardware Counters

## About

The application corresponds exactly to the application `tests/periph_timer`.
It is only for testing the timer implementation using hardware counters.
In fact, the application is nothing more than the `tests/periph_timer`
applicaton compiled with the following command:

```
USEMODULE=esp_hw_counter make -C tests/periph_timer
```

## Epxected Results

Since the application corresponds exactly to the application
`tests/periph_timer`, the expected results are the same as for
`tests/periph_timer`.
