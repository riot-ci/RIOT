#ifndef SPI_H
#define SPI_H

#ifndef MODULE_PERIPH_SPI
#error "No SPI support on your board"
#endif

#ifdef __cplusplus
#include "spiport.hpp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARDUINO_SPI_INTERFACE
#define ARDUINO_SPI_INTERFACE 0 /**< Number of the SPI dev to make available to Arduino code */
#endif

#ifdef __cplusplus
}
#endif

#endif /* SPI_H */
