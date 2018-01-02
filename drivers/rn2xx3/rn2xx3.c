/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_rn2xx3
 * @{
 *
 * @file
 * @brief       Driver implementation for the RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "assert.h"
#include "xtimer.h"
#include "rn2xx3_params.h"
#include "rn2xx3.h"
#include "rn2xx3_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Delay when resetting the device, 10ms
 */
#define RESET_DELAY                 (10UL * US_PER_MS)

/*
 * Interrupt callbacks
 */
static void _rx_cb(void *arg, uint8_t c)
{
    rn2xx3_t *dev = (rn2xx3_t *)arg;
    netdev_t* netdev = (netdev_t *)dev;

    /* Avoid overflow of module response buffer */
    if (dev->resp_size >= RN2XX3_MAX_BUF) {
        return;
    }

    /* The device closes the response with \r\n when done */
    if (c == '\r') {
        return;
    }
    else if (c == '\n') { /* response end delimiter */
        dev->resp_buf[dev->resp_size++] = '\0';
        if (dev->int_state == RN2XX3_INT_STATE_MAC_RX_MESSAGE) {
            /* RX state: closing RX buffer */
            dev->rx_buf[(dev->rx_size + 1) / 2] = 0;
            if (netdev->event_callback) {
                netdev->event_callback(netdev, NETDEV_EVENT_ISR);
            }
        }
        else if (dev->int_state == RN2XX3_INT_STATE_MAC_TX) {
            /* still in TX state: transmission complete but no data received */
            if (netdev->event_callback) {
                netdev->event_callback(netdev, NETDEV_EVENT_ISR);
            }
        }
        dev->resp_size = 0;
        dev->rx_size = 0;
        dev->resp_done = 1;
        mutex_unlock(&(dev->resp_lock));
    }
    else {
        switch (dev->int_state) {
            /* A successful mac TX command expects 2 kinds of replies:
                 * - mac_tx_ok: transmission done, no data received
                 * - mac_rx <port> <data>: transmission done, some data
                 *                         received on port <port>. */
            case RN2XX3_INT_STATE_MAC_TX:
                /* some data are available */
                dev->resp_buf[dev->resp_size++] = c;
                /* if module response ends with 'rx ' after 8 received chars,
                   the module starts receiving data */
                if (dev->resp_size == 8 && dev->resp_buf[4] == 'r'
                    && dev->resp_buf[5] == 'x' && dev->resp_buf[6] == ' ') {
                    /* next received chars correspond to the port number */
                    dev->int_state = RN2XX3_INT_STATE_MAC_RX_PORT;
                }
                break;

            case RN2XX3_INT_STATE_MAC_RX_PORT:
                dev->resp_buf[dev->resp_size++] = c;
                if (c == ' ') {
                    dev->int_state = RN2XX3_INT_STATE_MAC_RX_MESSAGE;
                }
                break;

            case RN2XX3_INT_STATE_MAC_RX_MESSAGE:
                /* read and convert RX data (received in hex chars) */
                if (c == ' ') {
                    dev->resp_buf[dev->resp_size++] = c;
                    break;
                }
                dev->rx_tmp_buf[dev->rx_size % 2] = c;

                /* We convert pairs of hex character to bytes on the fly to
                   save space in memory */
                if (dev->rx_size != 0 && dev->rx_size % 2) {
                    rn2xx3_hex_to_bytes(dev->rx_tmp_buf,
                                        &dev->rx_buf[(dev->rx_size - 1) / 2]);
                }
                dev->rx_size++;
                break;

            default:
                dev->resp_buf[dev->resp_size++] = c;
                break;
        }
    }
}

static void _sleep_timer_cb(void *arg)
{
    rn2xx3_t *dev = (rn2xx3_t *)arg;
    dev->int_state = RN2XX3_INT_STATE_IDLE;
    DEBUG("[rn2xx3] exit sleep\n");
}

/*
 * Driver's "public" functions
 */
void rn2xx3_setup(rn2xx3_t *dev, const rn2xx3_params_t *params)
{
    assert(dev && (params->uart < UART_NUMOF));

    /* initialize device parameters */
    memcpy(&dev->p, params, sizeof(rn2xx3_params_t));

    /* initialize pins */
    if (dev->p.pin_reset != GPIO_UNDEF) {
        gpio_init(dev->p.pin_reset, GPIO_OUT);
        gpio_set(dev->p.pin_reset);
    }
    /* UART is initialized later, since interrupts cannot be handled yet */
}

int rn2xx3_init(rn2xx3_t *dev)
{
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_RESET);

    /* initialize buffers and locks*/
    dev->resp_size = 0;
    dev->cmd_buf[0] = '\0';

    /* initialize UART and GPIO pins */
    if (uart_init(dev->p.uart, dev->p.baudrate, _rx_cb, dev) != UART_OK) {
        DEBUG("[rn2xx3] init: error initializing UART\n");
        return -ENXIO;
    }

    /* if reset pin is connected, do a hardware reset */
    if (dev->p.pin_reset != GPIO_UNDEF) {
        gpio_clear(dev->p.pin_reset);
        xtimer_usleep(RESET_DELAY);
        gpio_set(dev->p.pin_reset);
    }

    dev->sleep_timer.callback = _sleep_timer_cb;
    dev->sleep_timer.arg = dev;

    rn2xx3_sys_set_sleep_duration(dev, RN2XX3_DEFAULT_SLEEP);

    /* sending empty command to clear uart buffer */
    if (rn2xx3_write_cmd(dev) == RN2XX3_TIMEOUT) {
        DEBUG("[rn2xx3] init: initialization failed\n");
        return RN2XX3_TIMEOUT;
    }

    if (rn2xx3_mac_init(dev) != RN2XX3_OK) {
        DEBUG("[rn2xx3] mac: initialization failed\n");
        return RN2XX3_ERR_MAC_INIT;
    }

    DEBUG("[rn2xx3] init: initialization successful\n");
    return RN2XX3_OK;
}

int rn2xx3_sys_reset(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "sys reset");
    dev->cmd_buf[p] = 0;
    int ret = rn2xx3_write_cmd(dev);
    if (ret == RN2XX3_TIMEOUT || ret == RN2XX3_ERR_SLEEP_MODE) {
        DEBUG("[rn2xx3] reset: failed\n");
        return ret;
    }

    return RN2XX3_OK;
}

int rn2xx3_sys_factory_reset(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "sys factoryRESET");
    dev->cmd_buf[p] = 0;
    int ret = rn2xx3_write_cmd(dev);
    if (ret == RN2XX3_TIMEOUT || ret == RN2XX3_ERR_SLEEP_MODE) {
        DEBUG("[rn2xx3] factory reset: failed\n");
        return ret;
    }

    return RN2XX3_OK;
}

int rn2xx3_sys_sleep(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "sys sleep %lu", (unsigned long)dev->sleep);
    dev->cmd_buf[p] = 0;
    if (rn2xx3_write_cmd_no_wait(dev) == RN2XX3_ERR_INVALID_PARAM) {
        DEBUG("[rn2xx3] sleep: cannot put module in sleep mode\n");
        return RN2XX3_ERR_INVALID_PARAM;
    }

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_SLEEP);
    xtimer_set(&dev->sleep_timer, dev->sleep * US_PER_MS);

    return RN2XX3_OK;
}

int rn2xx3_mac_init(rn2xx3_t *dev)
{
    rn2xx3_mac_set_dr(dev, LORAMAC_DEFAULT_DR);
    rn2xx3_mac_set_tx_power(dev, LORAMAC_DEFAULT_TX_POWER);
    rn2xx3_mac_set_tx_port(dev, LORAMAC_DEFAULT_TX_PORT);
    rn2xx3_mac_set_tx_mode(dev, LORAMAC_DEFAULT_TX_MODE);
    rn2xx3_mac_set_adr(dev, LORAMAC_DEFAULT_ADR);
    rn2xx3_mac_set_retx(dev, LORAMAC_DEFAULT_RETX);
    rn2xx3_mac_set_linkchk_interval(dev, LORAMAC_DEFAULT_LINKCHK);
    rn2xx3_mac_set_rx1_delay(dev, LORAMAC_DEFAULT_RX1_DELAY);
    rn2xx3_mac_set_ar(dev, LORAMAC_DEFAULT_AR);
    rn2xx3_mac_set_rx2_dr(dev, LORAMAC_DEFAULT_RX2_DR);
    rn2xx3_mac_set_rx2_freq(dev, LORAMAC_DEFAULT_RX2_FREQ);

    rn2xx3_mac_set_dev_eui(dev, dev->p.loramac.dev_eui);
    rn2xx3_mac_set_app_eui(dev, dev->p.loramac.app_eui);
    rn2xx3_mac_set_app_key(dev, dev->p.loramac.app_key);
    rn2xx3_mac_set_app_skey(dev, dev->p.loramac.app_skey);
    rn2xx3_mac_set_nwk_skey(dev, dev->p.loramac.nwk_skey);
    rn2xx3_mac_set_dev_addr(dev, dev->p.loramac.dev_addr);

    return RN2XX3_OK;
}

int rn2xx3_mac_tx(rn2xx3_t *dev, uint8_t *payload, uint8_t payload_len)
{
    if (dev->int_state == RN2XX3_INT_STATE_SLEEP) {
        DEBUG("[rn2xx3] ABORT: device is in sleep mode\n");
        return RN2XX3_ERR_SLEEP_MODE;
    }

    rn2xx3_mac_tx_start(dev);
    for (unsigned i = 0; i < payload_len; ++i) {
        rn2xx3_cmd_append(dev, &payload[i], 1);
    }

    int ret = rn2xx3_mac_tx_finalize(dev);
    if (ret != RN2XX3_OK) {
        rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);
        DEBUG("[rn2xx3] TX command failed\n");
        return ret;
    }

    ret = rn2xx3_wait_reply(dev, RN2XX3_REPLY_DELAY_TIMEOUT);
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    return ret;
}

int rn2xx3_mac_join_network(rn2xx3_t *dev, uint8_t mode)
{
    /* With ABP join procedure, only save keys to EEPROM if device address is
       different from the current value stored */
    if (mode == LORAMAC_JOIN_ABP) {
        uint8_t addr[4];
        uint32_t eeprom_addr = 0, conf_addr = 0;
        rn2xx3_mac_get_dev_addr(dev, addr);
        eeprom_addr |= (uint32_t)addr[0] << 24;
        eeprom_addr |= (uint32_t)addr[1] << 16;
        eeprom_addr |= (uint32_t)addr[2] << 8;
        eeprom_addr |= (uint32_t)addr[3];

        conf_addr |= (uint32_t)dev->p.loramac.dev_addr[0] << 24;
        conf_addr |= (uint32_t)dev->p.loramac.dev_addr[1] << 16;
        conf_addr |= (uint32_t)dev->p.loramac.dev_addr[2] << 8;
        conf_addr |= (uint32_t)dev->p.loramac.dev_addr[3];

        if (conf_addr != eeprom_addr) {
            rn2xx3_mac_save(dev);
        }
    }

    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                  "mac join %s", (mode == LORAMAC_JOIN_OTAA) ? "otaa" : "abp");
    dev->cmd_buf[p] = 0;

    int ret = rn2xx3_write_cmd(dev);
    if (ret != RN2XX3_OK) {
        DEBUG("[rn2xx3] join procedure command failed\n");
        return ret;
    }

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_MAC_JOIN);

    ret = rn2xx3_wait_reply(dev,
                            LORAMAC_DEFAULT_JOIN_DELAY1 + \
                            LORAMAC_DEFAULT_JOIN_DELAY2);

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    return ret;
}

int rn2xx3_mac_save(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "mac save");
    dev->cmd_buf[p] = 0;
    return rn2xx3_write_cmd(dev);
}
