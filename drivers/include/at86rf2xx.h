/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_at86rf2xx AT86RF2xx based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for radio devices in Atmel's AT86RF2xx series.
 * The driver is aimed to work with all devices of this series.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for AT86RF2xx based drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_H
#define AT86RF2XX_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"

/* we need no peripherals for memory mapped radios */
//#if !defined(MODULE_AT86RFA1) && !defined(MODULE_AT86RFR2)
#include "periph/spi.h"
#include "periph/gpio.h"
//#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define AT86RF2XX_MAX_PKT_LENGTH        (IEEE802154_FRAME_LEN_MAX)

#if IS_USED(MODULE_AT86RFA1) || \
    IS_USED(MODULE_AT86RFR2)
#define AT86RF2XX_IN_MCU                (1)
#else
#define AT86RF2XX_IN_MCU                (0)
#endif

#if IS_USED(MODULE_PERIPH_SPI)
#define AT86RF2XX_NEED_SPI              (1)
#else
#define AT86RF2XX_NEED_SPI              (0)
#endif

#if IS_USED(MODULE_PERIPH_GPIO_IRQ)
#define AT86RF2XX_NEED_GPIO_IRQ         (1)
#else
#define AT86RF2XX_NEED_GPIO_IRQ         (0)
#endif

/**
 * @name    Flags for device internal states (see datasheet)
 * @{
 */
#define AT86RF2XX_STATE_P_ON           (0x00)     /**< initial power on */
#define AT86RF2XX_STATE_BUSY_RX        (0x01)     /**< busy receiving data (basic mode) */
#define AT86RF2XX_STATE_BUSY_TX        (0x02)     /**< busy transmitting data (basic mode) */
#define AT86RF2XX_STATE_FORCE_TRX_OFF  (0x03)     /**< force transition to idle */
#define AT86RF2XX_STATE_RX_ON          (0x06)     /**< listen mode (basic mode) */
#define AT86RF2XX_STATE_TRX_OFF        (0x08)     /**< idle */
#define AT86RF2XX_STATE_PLL_ON         (0x09)     /**< ready to transmit */
#define AT86RF2XX_STATE_SLEEP          (0x0f)     /**< sleep mode */
#define AT86RF2XX_STATE_BUSY_RX_AACK   (0x11)     /**< busy receiving data (extended mode) */
#define AT86RF2XX_STATE_BUSY_TX_ARET   (0x12)     /**< busy transmitting data (extended mode) */
#define AT86RF2XX_STATE_RX_AACK_ON     (0x16)     /**< wait for incoming data */
#define AT86RF2XX_STATE_TX_ARET_ON     (0x19)     /**< ready for sending data */
#define AT86RF2XX_STATE_IN_PROGRESS    (0x1f)     /**< ongoing state conversion */
/** @} */

/**
 * @name    Internal device option flags
 * @{
 */
#define AT86RF2XX_OPT_TELL_TX_START  (0x0001)       /**< notify MAC layer on TX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_TX_END    (0x0002)       /**< notify MAC layer on TX
                                                     *   finished */
#define AT86RF2XX_OPT_TELL_RX_START  (0x0004)       /**< notify MAC layer on RX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_RX_END    (0x0008)       /**< notify MAC layer on RX
                                                     *   finished */
#define AT86RF2XX_OPT_CSMA           (0x0010)       /**< CSMA active */
#define AT86RF2XX_OPT_PROMISCUOUS    (0x0020)       /**< promiscuous mode
                                                     *   active */
#define AT86RF2XX_OPT_PRELOADING     (0x0040)       /**< preloading enabled */
#define AT86RF2XX_OPT_AUTOACK        (0x0080)       /**< Auto ACK active */
#define AT86RF2XX_OPT_ACK_PENDING    (0x0100)       /**< ACK frames with data
                                                     *   pending */
/** @} */

/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct at86rf2xx_params {
#if AT86RF2XX_NEED_SPI || defined(DOXYGEN)
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
#endif
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} at86rf2xx_params_t;

/**
 * @brief   Device descriptor for AT86RF2XX radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct at86rf2xx_base {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
    uint16_t flags;                         /**< Device specific flags */
    uint8_t state;                          /**< current state of the radio */
    uint8_t tx_frame_len;                   /**< length of the current TX frame */
    uint8_t idle_state;                     /**< state to return to after sending */
    uint8_t pending_tx;                     /**< keep track of pending TX calls
                                                 this is required to know when to
                                                 return to @ref at86rf2xx_t::idle_state */
    uint8_t dev_type;                      /**<  AT86RF2XX device type */
} at86rf2xx_base_t;

/**
 * @brief An pointer to an instance of an actual AT86RF2XX
 * must safely be casted to a pointer of this type
 */
typedef struct at86rf2xx {
    at86rf2xx_base_t base;
    at86rf2xx_params_t params;
} at86rf2xx_t;

#if IS_USED(MODULE_AT86RF212B)
typedef struct at86rf212b_params {
    at86rf2xx_params_t base_params;
} at86rf212b_params_t;

typedef struct at86rf212b {
    at86rf2xx_base_t base;
    at86rf212b_params_t params;
    /* additional members */
    uint8_t page;
} at86rf212b_t;
#else
typedef at86rf2xx_params_t at86rf212b_params_t;
typedef at86rf2xx_t at86rf212b_t;
#endif

#if IS_USED(MODULE_AT86RF231)
typedef struct at86rf231_params {
    at86rf2xx_params_t base_params;
} at86rf231_params_t;

typedef struct at86rf231 {
    at86rf2xx_base_t base;
    at86rf231_params_t params;
} at86rf231_t;
#else
typedef at86rf2xx_params_t at86rf231_params_t;
typedef at86rf2xx_t at86rf231_t;
#endif

#if IS_USED(MODULE_AT86RF232)
typedef struct at86rf232_params {
    at86rf2xx_params_t base_params;
} at86rf232_params_t;

typedef struct at86rf232 {
    at86rf2xx_base_t base;
    at86rf232_params_t params;
    /* additional members */
    /* Only radios with the XAH_CTRL_2 register support frame retry reporting */
    uint8_t tx_retries;                 /**< Number of NOACK retransmissions */
} at86rf232_t;
#else
typedef at86rf2xx_params_t at86rf232_params_t;
typedef at86rf2xx_t at86rf232_t;
#endif

#if IS_USED(MODULE_AT86RF233)
typedef struct at86rf233_params {
    at86rf2xx_params_t base_params;
} at86rf233_params_t;

typedef struct at86rf233 {
    at86rf2xx_base_t base;
    at86rf233_params_t params;
    /* additional members */
    /* Only radios with the XAH_CTRL_2 register support frame retry reporting */
    uint8_t tx_retries;                 /**< Number of NOACK retransmissions */
} at86rf233_t;
#else
typedef at86rf2xx_params_t at86rf233_params_t;
typedef at86rf2xx_t at86rf233_t;
#endif

#if IS_USED(MODULE_AT86RFA1)
typedef struct at86rfa1 {
    at86rf2xx_base_t base;
    /* additional members */
    /*
     *  irq_status = IRQ_STATUS
     */
    uint8_t irq_status;                     /**< save irq status */
} at86rfa1_t;
#else
typedef at86rf2xx_t at86rfa1_t;
#endif

#if IS_USED(MODULE_AT86RFR2)
typedef struct at86rfr2 {
    at86rf2xx_base_t base;
    /* additional members */
    /* ATmega256rfr2 signals transceiver events with different interrupts
     * they have to be stored to mimic the same flow as external transceiver
     * Use irq_status to map saved interrupts of SOC transceiver,
     * as they clear after IRQ callback.
     *
     *  irq_status = IRQ_STATUS
     */
    uint8_t irq_status;                     /**< save irq status */
} at86rfr2_t;
#else
typedef at86rf2xx_t at86rfr2_t;
#endif

/**
 * @brief   Dynamic size look up for a AT86RF2XX device
 *
 * @param[in]   dev         device handle
 *
 * @return                  device structure size in bytes
 */
size_t at86rf2xx_get_size(const at86rf2xx_t *dev);

/**
 * @brief   Setup @p num transceiver devices of type AT86RF212B
 *
 * @param[out]   dev        array of AT86RF212B device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf212b_setup(at86rf212b_t *devs, const at86rf212b_params_t *params, uint8_t num);

/**
 * @brief   Setup @p num transceiver devices of type AT86RF231
 *
 * @param[out]   dev        array of AT86RF231 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf231_setup(at86rf231_t *devs, const at86rf231_params_t *params, uint8_t num);

/**
 * @brief   Setup @p num transceiver devices of type AT86RF232
 *
 * @param[out]   dev        array of AT86RF232 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf232_setup(at86rf232_t *devs, const at86rf232_params_t *params, uint8_t num);

/**
 * @brief   Setup @p num transceiver devices of type AT86RF233
 *
 * @param[out]   dev        array of AT86RF233 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf233_setup(at86rf233_t *devs, const at86rf233_params_t *params, uint8_t num);

/**
 * @brief   Setup @p num transceiver devices of type AT86RFA1
 *
 * @param[out]   dev        array of AT86RFA1 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rfa1_setup(at86rfa1_t *devs);

/**
 * @brief   Setup @p num transceiver devices of type AT86RFR2
 *
 * @param[out]   dev        array of AT86RFR2 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rfr2_setup(at86rfr2_t *devs);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in,out] dev       device to reset
 */
void at86rf2xx_reset(at86rf2xx_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in]   dev         device to read from
 * @param[out]  addr        the short address will be stored here
 *
 * @return                  the currently set (2-byte) short address
 */
void at86rf2xx_get_addr_short(const at86rf2xx_t *dev, network_uint16_t *addr);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in]   dev         device to read from
 * @param[out]  addr        the long address will be stored here
 *
 * @return                  the currently set (8-byte) long address
 */
void at86rf2xx_get_addr_long(const at86rf2xx_t *dev, eui64_t *addr);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr);

/**
 * @brief   Get the configured channel number of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel number
 */
uint8_t at86rf2xx_get_chan(const at86rf2xx_t *dev);

/**
 * @brief   Set the channel number of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] chan          channel number to set
 */
void at86rf2xx_set_chan(at86rf2xx_t *dev, uint8_t chan);

/**
 * @brief   Get the configured channel page of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel page
 */
uint8_t at86rf2xx_get_page(const at86rf2xx_t *dev);

/**
 * @brief   Set the channel page of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] page          channel page to set
 */
void at86rf2xx_set_page(at86rf2xx_t *dev, uint8_t page);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t at86rf2xx_get_pan(const at86rf2xx_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] pan           PAN ID to set
 */
void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev);

/**
 * @brief   Set the transmission power of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] txpower       transmission power in dBm
 */
void at86rf2xx_set_txpower(const at86rf2xx_t *dev, int16_t txpower);

/**
 * @brief   Get the configured receiver sensitivity of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured receiver sensitivity in dBm
 */
int16_t at86rf2xx_get_rxsensitivity(const at86rf2xx_t *dev);

/**
 * @brief   Set the receiver sensitivity of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] rxsens        rx sensitivity in dBm
 */
void at86rf2xx_set_rxsensitivity(const at86rf2xx_t *dev, int16_t rxsens);

/**
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t at86rf2xx_get_max_retries(const at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of retransmissions
 *
 * This setting specifies the number of attempts to retransmit a frame, when it
 * was not acknowledged by the recipient, before the transaction gets cancelled.
 * The maximum value is 7.
 *
 * @param[in] dev           device to write to
 * @param[in] max           the maximum number of retransmissions
 */
void at86rf2xx_set_max_retries(const at86rf2xx_t *dev, uint8_t max);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of channel access attempts per frame (CSMA)
 *
 * This setting specifies the number of attempts to access the channel to
 * transmit a frame. If the channel is busy @p retries times, then frame
 * transmission fails.
 * Valid values: 0 to 5, -1 means CSMA disabled
 *
 * @param[in] dev           device to write to
 * @param[in] retries       the maximum number of retries
 */
void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries);

/**
 * @brief   Set the min and max backoff exponent for CSMA/CA
 *
 * - Maximum BE: 0 - 8
 * - Minimum BE: 0 - [max]
 *
 * @param[in] dev           device to write to
 * @param[in] min           the minimum BE
 * @param[in] max           the maximum BE
 */
void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max);

/**
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2]);

/**
 * @brief   Get the CCA threshold value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the current CCA threshold value
 */
int8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev);

/**
 * @brief   Set the CCA threshold value
 *
 * @param[in] dev           device to write to
 * @param[in] value         the new CCA threshold value
 */
void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, int8_t value);

/**
 * @brief   Get the latest ED level measurement
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the last ED level
 */
int8_t at86rf2xx_get_ed_level(const at86rf2xx_t *dev);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in,out] dev       device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in,out] dev       device to change state of
 * @param[in] state         the targeted new state
 *
 * @return                  the previous state before the new state was set
 */
uint8_t at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state);

/**
 * @brief   Convenience function for simply sending data
 *
 * @note This function ignores the PRELOADING option
 *
 * @param[in,out] dev       device to use for sending
 * @param[in] data          data to send (must include IEEE802.15.4 header)
 * @param[in] len           length of @p data
 *
 * @return                  number of bytes that were actually send
 * @return                  0 on error
 */
size_t at86rf2xx_send(at86rf2xx_t *dev, const uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in,out] dev        device to prepare for sending
 */
void at86rf2xx_tx_prepare(at86rf2xx_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in,out] dev       device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 * @param[in] offset        offset used when writing data to internal buffer
 *
 * @return                  offset + number of bytes written
 */
size_t at86rf2xx_tx_load(at86rf2xx_t *dev, const uint8_t *data,
                         size_t len, size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void at86rf2xx_tx_exec(const at86rf2xx_t *dev);

/**
 * @brief   Perform one manual channel clear assessment (CCA)
 *
 * The CCA mode and threshold level depends on the current transceiver settings.
 *
 * @param[in]  dev          device to use
 *
 * @return                  true if channel is determined clear
 * @return                  false if channel is determined busy
 */
bool at86rf2xx_cca(at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_H */
/** @} */
