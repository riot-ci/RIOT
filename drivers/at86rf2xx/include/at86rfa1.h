/*
 * Copyright (C) 2019 OvGU Magdeburg
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
 * @brief       AT86RFA1
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RFA1_H
#define AT86RFA1_H

#if IS_USED(MODULE_AT86RFA1)

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const int16_t _231_232_a1_r2_tx_pow_to_dbm[16];
extern const uint8_t _231_232_a1_r2_dbm_to_tx_pow[21];
extern const int16_t _231_232_a1_r2_rx_sens_to_dbm[16];
extern const uint8_t _231_232_a1_r2_dbm_to_rx_sens[53];

/**
 * @brief   Maximum value to configure transmission power
 *          for AT86RFA1 transceivers
 */
#define AT86RFA1_TXPOWER_MAX                (20)
/**
 * @brief   Transmission power offset
 *          for AT86RFA1 transceivers
 */
#define AT86RFA1_TXPOWER_OFF                (17)
/**
 * @brief   Maximum receiver sensibility in dbm
 *          for AT86RFA1 transceivers
 */
#define AT86RFA1_MAX_RX_SENSITIVITY         (-48)
/**
 * @brief   Minimum receiver sensibility in dbm
 *          for AT86RFA1 transceivers
 */
#define AT86RFA1_MIN_RX_SENSITIVITY         (-100)
/**
 * @brief   AT86RFA1 transceiver partnumber
 */
#define AT86RFA1_PARTNUM                    (0x83)
/**
 * @brief   AT86RFA1 RSSI base value
 */
#define AT86RFA1_RSSI_BASE_VAL              (-90)
/**
 * @brief   AT86RFA1 minimum channel
 */
#define AT86RFA1_MIN_CHANNEL                (IEEE802154_CHANNEL_MIN)
/**
 * @brief   AT86RFA1 maximum channel
 */
#define AT86RFA1_MAX_CHANNEL                (IEEE802154_CHANNEL_MAX)
/**
 * @brief   AT86RFA1 default channel
 */
#define AT86RFA1_DEFAULT_CHANNEL            (IEEE802154_DEFAULT_CHANNEL)
/* Only page 0 is supported in the 2.4 GHz band */

/**
 * @brief   State transition delay from SLEEP to TRX_OFF
 *          for AT86RFA1 transceivers
 */
#define AT86RFA1_WAKEUP_DELAY               (210U)

/**
 * @brief   AT86RF2XX transceiver register address offset
 *          for ATmega128RFA1
 */
#define AT86RFA1_REGISTER_OFFSET            (0x140)

/**
 * @brief   Convert TX_PWR register value to actual transmission power in dbm
 *          for AT86RFA1 transceivers
 */
static inline int16_t
at86rfa1_tx_pow_to_dbm(const at86rfa1_t *dev, uint8_t txpower)
{
    (void)dev;
    return _231_232_a1_r2_tx_pow_to_dbm[txpower];
}
/**
 * @brief   Convert transmission power in dbm to TX_PWR register value
 *          for AT86RFA1 transceivers
 */
static inline uint8_t
at86rfa1_dbm_to_tx_pow(const at86rfa1_t *dev, int16_t dbm)
{
    (void)dev;
    dbm += AT86RFA1_TXPOWER_OFF;
    if (dbm < 0) {
        dbm = 0;
    }
    else if (dbm > AT86RFA1_TXPOWER_MAX) {
        dbm = AT86RFA1_TXPOWER_MAX;
    }
    return _231_232_a1_r2_dbm_to_tx_pow[dbm];
}
/**
 * @brief   Convert RX_PDT_LEVEL registe value to actual receiver sensibility in dbm
 *          for AT86RFA1 transceivers
 */
static inline int16_t
at86rfa1_rx_sens_to_dbm(const at86rfa1_t *dev, uint8_t rxsens)
{
    (void)dev;
    return _231_232_a1_r2_rx_sens_to_dbm[rxsens];
}
/**
 * @brief   Convert receiver sensibility to RX_PDT_LEVEL register value
 *          for AT86RFA1 transceivers
 */
static inline uint8_t
at86rfa1_dbm_to_rxsens(const at86rfa1_t *dev, int16_t dbm)
{
    (void)dev;
    if (dbm < AT86RFA1_MIN_RX_SENSITIVITY) {
        dbm = AT86RFA1_MIN_RX_SENSITIVITY;
    }
    else if (dbm > AT86RFA1_MAX_RX_SENSITIVITY) {
        dbm = AT86RFA1_MAX_RX_SENSITIVITY;
    }
    dbm -= AT86RFA1_MIN_RX_SENSITIVITY;
    return _231_232_a1_r2_dbm_to_rx_sens[dbm];
}

#ifdef __cplusplus
}
#endif

#endif /* #if IS_USED(MODULE_AT86RFA1) */

#endif /* AT86RFA1_H */
