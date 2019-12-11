/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Internal interfaces for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_INTERNAL_H
#define AT86RF2XX_INTERNAL_H

#include <stdint.h>

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum reset pulse width, refer p.190. We use 62us so
 *          that it is at least one tick on platforms with coarse xtimers
 */
#define AT86RF2XX_RESET_PULSE_WIDTH     (62U)

/**
 * @brief   The typical transition time to TRX_OFF after reset pulse is 26 us,
 *          refer to figure 7-8, p. 44. We use 62 us so that it is at least one
 *          tick on platforms that use a 16384 Hz oscillator or have slow start
 *          up times due to parasitic capacitance on the oscillator
 */
#define AT86RF2XX_RESET_DELAY           (62U)

/**
 * @brief   Maximum number of retransmissions
 *
 * See MAX_FRAME_RETRIES in XAH_CTRL_0
 */
#define AT86RF2XX_MAX_FRAME_RETRIES     (7)

/**
 * @brief Maximum number of CSMA retries
 *
 * See MAX_CSMA_RETRIES in XAH_CTRLL_0
 */
#define AT86RF2XX_MAX_CSMA_RETRIES      (5)

/**
 * @brief   Default TX power (0dBm)
 */
#define AT86RF2XX_DEFAULT_TXPOWER       (IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev);

/**
 * @brief   Make sure that device is not sleeping
 *
 * @param[in,out] dev   device to eventually wake up
 */
void at86rf2xx_assert_awake(at86rf2xx_t *dev);

/**
 * @brief   Trigger a hardware reset
 *
 * @param[in,out] dev   device to reset
 */
void at86rf2xx_hardware_reset(at86rf2xx_t *dev);


/**
 * @brief   Set PHY parameters based on channel and page number
 *
 * @param[in,out] dev   device to configure
 */
void at86rf2xx_configure_phy(at86rf2xx_t *dev);

/**
 * @brief   Read random data from the RNG
 *
 * @note    According to the data sheet this function only works properly in
 *          Basic Operation Mode. However, sporadic testing has shown that even
 *          in Extended Operation Mode this returns random enough data to be
 *          used as a seed for @ref sys_random if no cryptographically secure
 *          randomness is required.
 *          Any further use-case needs to be evaluated, especially if
 *          crypto-relevant randomness is required.
 *
 * @param[in] dev       device to configure
 * @param[out] data     buffer to copy the random data to
 * @param[in]  len      number of random bytes to store in data
 */
void at86rf2xx_get_random(const at86rf2xx_t *dev, uint8_t *data, size_t len);

void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_INTERNAL_H */
/** @} */
