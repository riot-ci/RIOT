# About
This is a test application for the ON Semiconductor lc709203F Battery Fuel Gauge
# Usage
Just enter the `make BOARD=??? flash` command in the `tests/driver_lc709203F/` folder.
Mak sure the `LC709203F_INT_PIN` is set in your boards periph_conf.h
# Results
The sensor will first test the low RSOC interrupt by setting the threshuld limit to 100%. Therefor a low RSOC interrupt should be triggered and the message "ALARM low RSOC" should be printed to the terminal. After that all mayor measurements will be printed and refereshed every 5s.
