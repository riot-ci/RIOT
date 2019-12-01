/*
 * Copyright (C) 2019 OvGU Magdeburf
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
 * @brief       AT86RF212B
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF212B_H
#define AT86RF212B_H

#if IS_USED(MODULE_AT86RF212B)

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t _212b_dbm_to_tx_pow_868[37];
extern const uint8_t _212b_dbm_to_tx_pow_915[37];
extern const int16_t _212b_rx_sens_to_dbm[16];
extern const uint8_t _212b_dbm_to_rx_sens[57];

/**
 * @brief   Maximum value to configure transmission power
 *          for AT86RF212B transceivers
 */
#define AT86RF212B_TXPOWER_MAX              (36)
/**
 * @brief   Transmission power offset
 *          for AT86RF212B transceivers
 */
#define AT86RF212B_TXPOWER_OFF              (25)
/**
 * @brief   Maximum receiver sensibility in dbm
 *          for AT86RF212B transceivers
 */
#define AT86RF212B_MAX_RX_SENSITIVITY       (-54)
/**
 * @brief   Minimum receiver sensibility in dbm
 *          for AT86RF212B transceivers
 */
#define AT86RF212B_MIN_RX_SENSITIVITY       (-110)
/**
 * @brief   AT86RF212B transceiver partnumber
 */
#define AT86RF212B_PARTNUM                  (0x07)
/**
 * @brief   AT86RF212B RSSI base value
 */
#define AT86RF212B_RSSI_BASE_VAL            (-98) /* approximation for [-100, -97]*/
/**
 * @brief   AT86RF212B minimum channel
 */
#define AT86RF212B_MIN_CHANNEL              (IEEE802154_CHANNEL_MIN_SUBGHZ)
/**
 * @brief   AT86RF212B maximum channel
 */
#define AT86RF212B_MAX_CHANNEL              (IEEE802154_CHANNEL_MAX_SUBGHZ)
/**
 * @brief   AT86RF212B default channel
 */
#define AT86RF212B_DEFAULT_CHANNEL          (IEEE802154_DEFAULT_SUBGHZ_CHANNEL)
/* Page 2 is O-QPSK 100 kbit/s (channel 0), or 250 kbit/s (channels 1-10) */
/**
 * @brief   AT86RF212B default page
 */
#define AT86RF212B_DEFAULT_PAGE             (IEEE802154_DEFAULT_SUBGHZ_PAGE)
/**
 * @brief   State transition delay from SLEEP to TRX_OFF
 *          for AT86RF212B transceivers
 */
#define AT86RF212B_WAKEUP_DELAY             (420U)

/**
 * @brief   Convert TX_PWR register value to actual transmission power in dbm
 *          for AT86RF212B transceivers
 */
static inline int16_t
at86rf212b_tx_pow_to_dbm(const at86rf212b_t *dev, uint8_t txpower)
{
    if (dev->page == 0 || dev->page == 2) {
        const uint8_t *dbm_to_tx_pow;
        size_t nelem;

        if (dev->base.netdev.chan == 0) {
            /* Channel 0 is 868.3 MHz */
            dbm_to_tx_pow = &_212b_dbm_to_tx_pow_868[0];
            nelem = ARRAY_SIZE(_212b_dbm_to_tx_pow_868);
        }
        else {
            /* Channels 1+ are 915 MHz */
            dbm_to_tx_pow = &_212b_dbm_to_tx_pow_915[0];
            nelem = ARRAY_SIZE(_212b_dbm_to_tx_pow_915);
        }

        for (size_t i = 0; i < nelem; ++i) {
            if (dbm_to_tx_pow[i] == txpower) {
                return (i - AT86RF212B_TXPOWER_OFF);
            }
        }
    }

    return 0;
}
/**
 * @brief   Convert transmission power in dbm to TX_PWR register value
 *          for AT86RF212B transceivers
 */
static inline uint8_t
at86rf212b_dbm_to_tx_pow(const at86rf212b_t *dev, int16_t dbm)
{
    dbm += AT86RF212B_TXPOWER_OFF;
    if (dbm < 0) {
        dbm = 0;
    }
    else if (dbm > AT86RF212B_TXPOWER_MAX) {
        dbm = AT86RF212B_TXPOWER_MAX;
    }
    if (dev->base.netdev.chan == 0) {
        return _212b_dbm_to_tx_pow_868[dbm];
    }
    else {
        return _212b_dbm_to_tx_pow_915[dbm];
    }
}
/**
 * @brief   Convert RX_PDT_LEVEL registe value to actual receiver sensibility in dbm
 *          for AT86RF212B transceivers
 */
static inline int16_t
at86rf212b_rx_sens_to_dbm(const at86rf212b_t *dev, uint8_t rxsens)
{
    (void)dev;
    return _212b_rx_sens_to_dbm[rxsens];
}
/**
 * @brief   Convert receiver sensibility to RX_PDT_LEVEL register value
 *          for AT86RF212B transceivers
 */
static inline uint8_t
at86rf212b_dbm_to_rxsens(const at86rf212b_t *dev, int16_t dbm)
{
    (void)dev;
    if (dbm < AT86RF212B_MIN_RX_SENSITIVITY) {
        dbm = AT86RF212B_MIN_RX_SENSITIVITY;
    }
    else if (dbm > AT86RF212B_MAX_RX_SENSITIVITY) {
        dbm = AT86RF212B_MAX_RX_SENSITIVITY;
    }
    dbm -= AT86RF212B_MIN_RX_SENSITIVITY;
    return _212b_dbm_to_rx_sens[dbm];
}

#ifdef __cplusplus
}
#endif

#endif /* IS_USED(MODULE_AT86RF212B) */

#endif /* AT86RF212B_H */
