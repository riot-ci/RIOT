/**
* @defgroup     
* @ingroup      pkg
* @brief
* @{
*
* @file
* @brief
*
* Some adress defines and device descriptor for Atmel CryptoAuth devices
*
* @author      
*
*/

#ifndef CRYPTOAUTHLIB_CONTRIB_H
#define CRYPTOAUTHLIB_CONTRIB_H

#include "periph/i2c.h"
#include "cryptoauthlib.h"

#ifdef __cpluspluses
extern "C" {
#endif

/* For ATECC508A*/
#ifndef ATCA_I2C_ADR
#define ATCA_I2C_ADR (0xC0)    /**< Default device adress is 0xC0. We need to shift it by 1, to ignore lsb (rw bit) */
#endif

#ifndef ATCA_GPIO_WAKE
#define ATCA_GPIO_WAKE (GPIO_PIN(0, 16))     /**< Default GPIO pin to map SDA for wake function */
#endif

#define ATCA_DATA_ADR   (0x03)            /**< Word Address to read and write to data area */

/**
 * @brief   Device descriptor contains ATCAIfaceCfg structure
 */
typedef struct {
    ATCAIfaceCfg params;        /**< device configuration */
} atca_t;

#ifdef __cplusplus
}
#endif

#endif /* CRYPTOAUTHLIB_CONTRIB_H */
/** @} */