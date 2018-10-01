# About

Test application for the HMC5883L 3-axis digital compass

# Usage

The test application demonstrates the use of the HMC5883L. It uses the
default configuration parameters.

- Continuous measurement at a Data Output Rate (DOR) of 15 Hz
- Normal mode, no biasing
- Gain 1090 LSb/Gs
- No averaging of data samples

The application can use the different approaches to get new data:

- using the #hmc5883l_read function at a lower rate than the the DOR
- using the data-ready interrupt (**DRDY**)

The data-ready interrupt (**DRDY) of the sensor is enabled permanently.
The application has only to configure and initialize the GPIO to which the
interrupt signal is connected. This is done by defining
```USE_HMC5883L_DRDY``` and overrding the default configuration
parameter ```HMC5883L_PARAM_DRDY``` if necessary, for example:

```
CFLAGS="-DUSE_HMC5883L_DRDY -DHMC5883L_PARAM_DRDY=GPIO12" \
make flash -C tests/driver_hmc5883l BOARD=...
```
