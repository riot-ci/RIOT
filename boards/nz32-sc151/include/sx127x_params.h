/*
 * Copyright (C) 2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nz32-sc151
 * @{
 *
 * @file
 * @brief       sx127x configuration for nz32-sc15 using modtronix inAir9
 *              LoRa module
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

 #ifndef SX127X_PARAMS_H
 #define SX127X_PARAMS_H

 #include "board.h"
 #include "sx127x.h"

 #ifdef __cplusplus
 extern "C" {
 #endif

 /**
  * @name    Set default configuration parameters for the SX127X driver
  *          Pins are adapted to nz32-sc151 using modtronix inAir9 Lora
  *          module.
  * @{
  */
 #ifndef SX127X_PARAM_SPI
 #define SX127X_PARAM_SPI                    (SPI_DEV(0))
 #endif

 #ifndef SX127X_PARAM_SPI_SPEED
 #define SX127X_PARAM_SPI_SPEED              (SPI_CLK_1MHZ)
 #endif

 #ifndef SX127X_PARAM_SPI_MODE
 #define SX127X_PARAM_SPI_MODE               (SPI_MODE_0)
 #endif

 #ifndef SX127X_PARAM_SPI_NSS
 #define SX127X_PARAM_SPI_NSS                GPIO_PIN(PORT_C, 8)      /* CS */
 #endif

 #ifndef SX127X_PARAM_RESET
 #define SX127X_PARAM_RESET                  GPIO_PIN(PORT_A, 9)      /* RT */
 #endif

 #ifndef SX127X_PARAM_DIO0
 #define SX127X_PARAM_DIO0                   GPIO_PIN(PORT_B, 0)      /* D0 */
 #endif

 #ifndef SX127X_PARAM_DIO1
 #define SX127X_PARAM_DIO1                   GPIO_PIN(PORT_B, 1)      /* D1 */
 #endif

 #ifndef SX127X_PARAM_DIO2
 #define SX127X_PARAM_DIO2                   GPIO_PIN(PORT_C, 6)      /* D2 */
 #endif

 #ifndef SX127X_PARAM_DIO3
 #define SX127X_PARAM_DIO3                   GPIO_PIN(PORT_A, 10)     /* D3 */
 #endif

 #define SX127X_PARAMS_DEFAULT               { .spi       = SX127X_PARAM_SPI,     \
                                               .nss_pin   = SX127X_PARAM_SPI_NSS, \
                                               .reset_pin = SX127X_PARAM_RESET,   \
                                               .dio0_pin  = SX127X_PARAM_DIO0,    \
                                               .dio1_pin  = SX127X_PARAM_DIO1,    \
                                               .dio2_pin  = SX127X_PARAM_DIO2,    \
                                               .dio3_pin  = SX127X_PARAM_DIO3 }
 /**@}*/

 /**
  * @brief   SX127X configuration
  */
 static const sx127x_params_t sx127x_params[] =
 {
 #ifdef SX127X_PARAMS_BOARD
     SX127X_PARAMS_BOARD,
 #else
     SX127X_PARAMS_DEFAULT,
 #endif
 };

 #ifdef __cplusplus
 }
 #endif

 #endif /* SX127X_PARAMS_H */
 /** @} */
