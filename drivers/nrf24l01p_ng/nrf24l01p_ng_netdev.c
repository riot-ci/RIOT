/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Implementation of RIOT's netdev_driver API
 *          for the NRF24L01+ (NG) transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <errno.h>
#include <string.h>
#include <assert.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "kernel_defines.h"
#include "iolist.h"
#include "irq.h"
#include "luid.h"
#include "mutex.h"
#include "net/eui64.h"
#include "net/netdev.h"
#include "xtimer.h"

#include "gnrc_netif_nrf24l01p_ng.h"
#include "nrf24l01p_ng_constants.h"
#include "nrf24l01p_ng_registers.h"
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_states.h"
#include "nrf24l01p_ng_netdev.h"
#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
#include "nrf24l01p_ng_diagnostics.h"
#endif

#define NRF24L01P_NG_FLG_IRQ \
    (NRF24L01P_NG_FLG_MAX_RT | NRF24L01P_NG_FLG_TX_DS | NRF24L01P_NG_FLG_RX_DR)

static
int _init(netdev_t *netdev);
static
int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static
int _send(netdev_t *netdev, const iolist_t *iolist);
static
void _isr(netdev_t *netdev);
static
int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static
int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len);

const netdev_driver_t nrf24l01p_ng_driver = {
    .init = _init,
    .recv = _recv,
    .send = _send,
    .isr = _isr,
    .get = _get,
    .set = _set
};

static inline
void _trigger_send(const nrf24l01p_ng_t *dev)
{
    gpio_set(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_NG_DELAY_US_CE_HIGH_PULSE);
    gpio_clear(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_NG_DELAY_US_TX_SETTLING);
}

static
int _assert_awake(const nrf24l01p_ng_t *dev)
{
    uint8_t config = 0;
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_CONFIG, &config, 1);
    return config & NRF24L01P_NG_FLG_PWR_UP;
}

static
netopt_state_t _state_to_netif(nrf24l01p_ng_state_t state)
{
    if (state == NRF24L01P_NG_STATE_POWER_DOWN) {
        return NETOPT_STATE_SLEEP;
    }
    if (state == NRF24L01P_NG_STATE_STANDBY_1) {
        return NETOPT_STATE_STANDBY;
    }
    if (state == NRF24L01P_NG_STATE_STANDBY_2) {
        return NETOPT_STATE_TX;
    }
    if (state == NRF24L01P_NG_STATE_TX_MODE) {
        return NETOPT_STATE_TX;
    }
    if (state == NRF24L01P_NG_STATE_RX_MODE) {
        return NETOPT_STATE_RX;
    }
    return NETOPT_STATE_OFF; /* error */
}

nrf24l01p_ng_state_t _state_from_netif(netopt_state_t state)
{
    if (state == NETOPT_STATE_SLEEP) {
        return NRF24L01P_NG_STATE_POWER_DOWN;
    }
    if (state == NETOPT_STATE_STANDBY) {
        return NRF24L01P_NG_STATE_STANDBY_1;
    }
    if (state == NETOPT_STATE_TX) {
        return NRF24L01P_NG_STATE_TX_MODE;
    }
    if (state == NETOPT_STATE_RX) {
        return NRF24L01P_NG_STATE_RX_MODE;
    }
    return NRF24L01P_NG_STATE_UNDEFINED;
}

static
void nrf24l01p_ng_irq_handler(void *_dev)
{
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)_dev;
    /* Once the IRQ pin has triggered,
       do not congest the thread´s
       message queue with IRQ events */
    gpio_irq_disable(dev->params.pin_irq);
    DEBUG_PUTS("[nrf24l01p_ng] IRQ\n");
    netdev_trigger_event_isr((netdev_t *)dev);
}

static
void _isr_max_rt(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS MAX_RT\n");
    nrf24l01p_ng_flush_tx(dev);
}

static
void _isr_rx_dr(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS RX_DR\n");
}

static
void _isr_tx_ds(const nrf24l01p_ng_t *dev)
{
    assert(dev->state == NRF24L01P_NG_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_NG_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_NG_STATE_RX_MODE   ||
           dev->state == NRF24L01P_NG_STATE_TX_MODE);
    DEBUG_PUTS("[nrf24l01p_ng] IRS TX_DS\n");
}

static
int _init(netdev_t *netdev)
{
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;
    if (dev->params.config.cfg_data_rate >= NRF24L01P_NG_RF_DR_NUM_OF ||
        dev->params.config.cfg_crc == NRF24L01P_NG_CRC_0BYTE ||
        dev->params.config.cfg_channel >= NRF24L01P_NG_NUM_CHANNELS) {
        return -ENOTSUP;
    }
    if (spi_init_cs(dev->params.spi, dev->params.pin_cs) != SPI_OK) {
        DEBUG_PUTS("[nrf24l01p_ng] _init(): spi_init_cs() failed\n");
        return -EIO;
    }
    if (gpio_init(dev->params.pin_ce, GPIO_OUT) < 0) {
        DEBUG_PUTS("[nrf24l01p_ng] _init(): gpio_init() failed\n");
        return -EIO;
    }
    gpio_clear(dev->params.pin_ce);
    if (nrf24l01p_ng_acquire(dev) < 0) {
        DEBUG_PUTS("[nrf24l01p_ng] _init(): nrf24l01p_ng_acquire() failed\n");
        return -EIO;
    }
    if (dev->state != NRF24L01P_NG_STATE_POWER_DOWN) {
        nrf24l01p_ng_transition_to_power_down(dev);
    }
    nrf24l01p_ng_flush_tx(dev);
    nrf24l01p_ng_flush_rx(dev);
    uint8_t aw = NRF24L01P_NG_ADDR_WIDTH;
    uint8_t bc[] = NRF24L01P_NG_BROADCAST_ADDR;
    luid_get_lb(dev->urxaddr.rxaddrpx.rx_p0, aw);
     /* "The LSByte must be unique for all six pipes" [datasheet p.38] */
    if (dev->urxaddr.rxaddrpx.rx_p0[aw - 1] == bc[aw - 1]) {
        luid_get_lb(dev->urxaddr.rxaddrpx.rx_p0, aw);
    }
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P0,
                           dev->urxaddr.rxaddrpx.rx_p0, aw);
    memcpy(dev->urxaddr.rxaddrpx.rx_p1, bc, aw);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P1,
                           dev->urxaddr.rxaddrpx.rx_p1, aw);
    uint8_t setup_aw = NRF24L01P_NG_FLG_AW(nrf24l01p_ng_valtoe_aw(aw));
    uint8_t rf_setup =
        NRF24L01P_NG_FLG_RF_DR(dev->params.config.cfg_data_rate) |
        NRF24L01P_NG_FLG_RF_PWR(dev->params.config.cfg_tx_power);
    uint8_t setup_retr =
        NRF24L01P_NG_FLG_ARD(dev->params.config.cfg_retr_delay) |
        NRF24L01P_NG_FLG_ARC(dev->params.config.cfg_max_retr);
    uint8_t rf_ch = NRF24L01P_NG_FLG_RF_CH(dev->params.config.cfg_channel);
    uint8_t en_rxaddr = NRF24L01P_NG_FLG_ERX_P0 | NRF24L01P_NG_FLG_ERX_P1;
    uint8_t config = NRF24L01P_NG_FLG_CRCO(dev->params.config.cfg_crc);
    uint8_t features = NRF24L01P_NG_FLG_EN_DYN_ACK |
                       NRF24L01P_NG_FLG_EN_DPL     |
                       NRF24L01P_NG_FLG_EN_ACK_PAY;
    uint8_t en_aa = NRF24L01P_NG_FLG_ENAA_P0 |NRF24L01P_NG_FLG_ENAA_P1;
    uint8_t endp = NRF24L01P_NG_FLG_DPL_P0 | NRF24L01P_NG_FLG_DPL_P1;
    uint8_t tx_addr[] = NRF24L01P_NG_DEFAULT_TX_ADDR;
    uint8_t status = NRF24L01P_NG_FLG_IRQ;
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_SETUP_AW, &setup_aw, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RF_SETUP, &rf_setup, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_SETUP_RETR, &setup_retr, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RF_CH, &rf_ch, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_EN_RXADDR, &en_rxaddr, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_CONFIG, &config, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_FEATURES, &features, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_EN_AA, &en_aa, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_DYNPD, &endp, 1);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_TX_ADDR, tx_addr,
                           sizeof(tx_addr));
    /* clear interrupts */
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_STATUS, &status, 1);
    nrf24l01p_ng_transition_to_standby_1(dev);
#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
    nrf24l01p_ng_diagnostics_print_all_regs(dev);
    nrf24l01p_ng_diagnostics_print_dev_info(dev);
#endif
    if (!_assert_awake(dev)) {
        nrf24l01p_ng_release(dev);
        return -ENODEV;
    }
    nrf24l01p_ng_transition_to_rx_mode(dev);
    nrf24l01p_ng_release(dev);
    if (gpio_init_int(dev->params.pin_irq, GPIO_IN, GPIO_FALLING,
                      nrf24l01p_ng_irq_handler, dev) < 0) {
        DEBUG_PUTS("[nrf24l01p_ng] _init(): gpio_init_int() failed\n");
        return -EIO;
    }
    return 0;
}

/**
 * @brief   NRF24L01+ @ref netdev_driver_t::recv routine
 *
 * @pre @see nrf24l01p_ng_acquire must have been called before.
 * @pre Interrupts should be disabled
 *
 * The SPI bus acquirement in this function because it is called from
 * @ref netdev_driver_t::isr, possibly for multiple times. If another
 * device acquired the SPI bus within the ISR, the ISR would block
 * until that device releases the bus.
 *
 * @param[in] netdev        Abstract network device handle
 * @param[out] buf          Rx buffer
 * @param[in] len           Size of Rx buffer
 * @param[out]              LQI and RSSI information (unused)
 *
 * @return                  Size of received frame in @p buf
 * @return                  Upper estimation of the frame width,
 *                          if @p buf == NULL and len == 0
 * @return                  Actual frame width,
 *                          if @p buf == NULL and @p len != 0
 *                          (frame is NOT dropped)
 * @retval -ENOBUFS         @p buf != NULL and @p len < actual frame width
 *                          (frame is dropped)
 * @retval -EINVAL          @p buf == NULL
 *                          (and none of the above cases are true)
 * @retval -ENOTSUP         Malformed header
 * @retval 0                No data to read from Rx FIFO
 */
static
int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info; /* nrf24l01+ supports neither lqi nor rssi */

    /* return upper estaimation bound of frame size */
    if (!buf && !len) {
        DEBUG_PUTS("[nrf24l01p_ng] Return upper frame estimation");
        return NRF24L01P_NG_ADDR_WIDTH + NRF24L01P_NG_MAX_PAYLOAD_WIDTH;
    }
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;
    uint8_t pl_width;
    uint8_t status = nrf24l01p_ng_read_rx_pl_width(dev, &pl_width);
    uint8_t pno = NRF24L01P_NG_VAL_RX_P_NO(status);
    if (!pl_width ||
        pl_width > NRF24L01P_NG_MAX_PAYLOAD_WIDTH ||
        pno >= NRF24L01P_NG_PX_NUM_OF) {
        DEBUG_PUTS("[nrf24l01p_ng] RX error, flush RX FIFO\n");
/* In some rare cases the RX payload width (R_RX_PL_WID) exceeds
   the maximum of 32 bytes. In that case it must be flushed.
   See https://devzone.nordicsemi.com/f/nordic-q-a/26489/nrf24l01-the-length-of-received-data-exceed-32
   and https://www.mikrocontroller.net/articles/NRF24L01_Tutorial */
        nrf24l01p_ng_flush_rx(dev);
        return 0;
    }
    uint8_t dst_addr[NRF24L01P_NG_ADDR_WIDTH];
    if (pno == NRF24L01P_NG_P0) {
        memcpy(dst_addr, dev->urxaddr.rxaddrpx.rx_p0, sizeof(dst_addr));
    }
    else {
        memcpy(dst_addr, dev->urxaddr.rxaddrpx.rx_p1, sizeof(dst_addr));
        if (pno > NRF24L01P_NG_P1) {
            dst_addr[NRF24L01P_NG_ADDR_WIDTH - 1] =
                dev->urxaddr.arxaddr.rx_addr_short[pno - 2];
        }
    }
    uint8_t frame_len = sizeof(dst_addr) + pl_width;
    /* do NOT drop frame and return exact frame size */
    if (!buf) {
        DEBUG_PUTS("[nrf24l01p_ng] Return exact frame length\n");
        return frame_len;
    }
    /* drop frame, content in buf becomes invalid and return -ENOBUFS */
    if (len < frame_len) {
        DEBUG("[nrf24l01p_ng] Buffer too small: %u < %u, dropping frame\n",
              len, frame_len);
        uint8_t garbage[pl_width];
        nrf24l01p_ng_read_rx_payload(dev, garbage, pl_width);
        return -ENOBUFS;
    }
    /* get received frame */
    DEBUG_PUTS("[nrf24l01p_ng] Handle received frame\n");
    uint8_t *frame = (uint8_t *)buf;
    memcpy(frame, dst_addr, sizeof(dst_addr));
    frame += sizeof(dst_addr);
    nrf24l01p_ng_read_rx_payload(dev, frame, pl_width);
#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
    nrf24l01p_ng_diagnostics_print_frame(dev, (uint8_t *)buf, frame_len);
#endif
    DEBUG("[nrf24l01p_ng] Received frame length: %u\n", frame_len);
    return (int)frame_len;
}

/**
 * @brief   NRF24L01+ @ref netdev_driver_t::send routine
 *
 * @param[in] netdev        Abstract network device handle
 * @param[in] iolist        Linked list of data to be sent, where
 *                          the base must be the destination address
 *
 * @return                  Size of sent payload
 * @retval -ENOTSUP         @p iolist had no base and no next link,
 *                          or address was too big, or too short
 * @retval -EAGAIN          Pending date has been sent first
 * @retval -E2BIG           Resulting frame from iolist was too big to be sent
 */
static
int _send(netdev_t *netdev, const iolist_t *iolist)
{
    assert(netdev && iolist);
    if (!(iolist->iol_base) || !(iolist->iol_next)) {
        DEBUG_PUTS("[nrf24l01p_ng] No Tx address or no payload\n");
        return -ENOTSUP;
    }
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;
    uint8_t pl_width = 0;
    uint8_t bcast_addr[] = NRF24L01P_NG_BROADCAST_ADDR;
    uint8_t payload[NRF24L01P_NG_MAX_PAYLOAD_WIDTH];
    nrf24l01p_ng_acquire(dev);
    uint8_t fifo_status;
    uint8_t status =
        nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_FIFO_STATUS,
                              &fifo_status, 1);
    if (status & NRF24L01P_NG_FLG_IRQ) {
        DEBUG_PUTS("[nrf24l01p_ng] Handle pending IRQ, before sending new data\n");
        nrf24l01p_ng_release(dev);
        _isr(&dev->netdev);
        return -EAGAIN;
    }
    if (fifo_status & NRF24L01P_NG_FLG_TX_FULL_) {
        nrf24l01p_ng_flush_tx(dev);
    }
    uint8_t *dst_addr = iolist->iol_base;
    uint8_t dst_addr_len = iolist->iol_len;
    if (dst_addr_len > NRF24L01P_NG_MAX_ADDR_WIDTH ||
        dst_addr_len < NRF24L01P_NG_MIN_ADDR_WIDTH) {
        nrf24l01p_ng_release(dev);
        DEBUG("[nrf24l01p_ng] Destination address has an invalid length: %u\n",
              dst_addr_len);
        return -ENOTSUP;
    }
    for (const iolist_t *iol = iolist->iol_next; iol; iol = iol->iol_next) {
        if (pl_width + iol->iol_len > sizeof(payload)) {
            nrf24l01p_ng_release(dev);
            DEBUG_PUTS("[nrf24l01p_ng] frame too big\n");
            return -E2BIG;
        }
        memcpy(payload + pl_width, iol->iol_base, iol->iol_len);
        pl_width += iol->iol_len;
    }
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_TX_ADDR,
                           dst_addr, dst_addr_len);
    if (!memcmp(dst_addr, bcast_addr, dst_addr_len)) {
        /* do not expect ACK for broadcast */
        nrf24l01p_ng_write_tx_pl_no_ack(dev, payload, pl_width);
    }
    else {
        nrf24l01p_ng_write_tx_payload(dev, payload, pl_width);
        /* A PTX node must change pipe 0 Rx address to Tx address
         * in order to receive ACKs.
         * If node switches back to Rx mode, pipe 0 Rx address
         * must be restored from params. */
        uint8_t setup_aw =
            NRF24L01P_NG_FLG_AW(nrf24l01p_ng_valtoe_aw(dst_addr_len));
        nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P0,
                               dst_addr, dst_addr_len);
        nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_SETUP_AW, &setup_aw, 1);
    }
    if (dev->state != NRF24L01P_NG_STATE_TX_MODE &&
        dev->state != NRF24L01P_NG_STATE_STANDBY_2) {
        dev->idle_state = dev->state;
        if (dev->state != NRF24L01P_NG_STATE_STANDBY_1) {
            nrf24l01p_ng_transition_to_standby_1(dev);
        }
        nrf24l01p_ng_transition_to_tx_mode(dev);
    }
    nrf24l01p_ng_release(dev);
    _trigger_send(dev);
    DEBUG("[nrf24l01p_ng] Sending %u bytes\n", pl_width);
    return (int)pl_width;
}

/**
 * @brief   NRF24L01+ @ref netdev_driver_t::isr
 *
 * @param[in] netdev        Abstract network device
 */
static
void _isr(netdev_t *netdev)
{
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;

    gpio_irq_enable(dev->params.pin_irq);
    nrf24l01p_ng_acquire(dev);
    uint8_t status = nrf24l01p_ng_get_status(dev);
    if (status & NRF24L01P_NG_FLG_MAX_RT) {
        _isr_max_rt(dev);
        netdev->event_callback(netdev, NETDEV_EVENT_TX_NOACK);
    }
    if (status & NRF24L01P_NG_FLG_TX_DS) {
        _isr_tx_ds(dev);
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    if (status & NRF24L01P_NG_FLG_RX_DR) {
        _isr_rx_dr(dev);
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    /* clear interrupt flags */
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_STATUS, &status, 1);
    uint8_t fifo_status;
    status =
        nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_FIFO_STATUS,
                              &fifo_status, 1);
    /* read all RX data */
    if (dev->state == NRF24L01P_NG_STATE_RX_MODE) {
        while (!(fifo_status & NRF24L01P_NG_FLG_RX_EMPTY)) {
            DEBUG_PUTS("[nrf24l01p_ng] ISR: read pending Rx frames\n");
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            status = NRF24L01P_NG_FLG_RX_DR;
            nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_STATUS, &status, 1);
            status = nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_FIFO_STATUS,
                                           &fifo_status, 1);
        }
        nrf24l01p_ng_release(dev);
    }
    else {
        if (dev->state == NRF24L01P_NG_STATE_TX_MODE) {
            /* frame in FIFO is not an ACK */
            if (!(fifo_status & NRF24L01P_NG_FLG_TX_EMPTY)) {
                nrf24l01p_ng_release(dev);
                _trigger_send(dev);
                return;
            }
        }
        /* no more data to transmit */
        if (dev->state != NRF24L01P_NG_STATE_STANDBY_1) {
            nrf24l01p_ng_transition_to_standby_1(dev);
        }
        /* go to idle state */
        if (dev->idle_state != NRF24L01P_NG_STATE_STANDBY_1) {
            if (dev->idle_state == NRF24L01P_NG_STATE_POWER_DOWN) {
                nrf24l01p_ng_transition_to_power_down(dev);
            }
            else {
                dev->idle_state = NRF24L01P_NG_STATE_RX_MODE;
                nrf24l01p_ng_transition_to_rx_mode(dev);
            }
        }
        nrf24l01p_ng_release(dev);
    }
}

/**
 * @brief   @ref netdev_driver_t::get
 *
 * @param[in] netdev        Abstract network device
 * @param[in] opt           netdev option type
 * @param[out] val          Option value
 * @param[in] max_len       Maximum option length
 *
 * @return                  Size of written option value
 * @retval -ENOTSUP         Unsupported netdev option @p opt
 */
static
int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;

    switch (opt) {
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = NRF24L01P_NG_ADDR_WIDTH;
            return sizeof(uint16_t);
        } break;
        case NETOPT_ADDRESS: {
            assert(max_len >= NRF24L01P_NG_ADDR_WIDTH);
            memcpy(val, dev->urxaddr.rxaddrpx.rx_p0,
                   NRF24L01P_NG_ADDR_WIDTH);
            return NRF24L01P_NG_ADDR_WIDTH;
        } break;
        case NETOPT_AUTOACK: {
            assert(max_len == sizeof(netopt_enable_t));
            /* mandatory for Enhanced ShockBurst */
            *((netopt_enable_t *)val) = NETOPT_ENABLE;
            return sizeof(netopt_enable_t);
        } break;
        case NETOPT_CHANNEL: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = (uint16_t)nrf24l01p_ng_get_channel(dev);
            return sizeof(uint16_t);
        } break;
        case NETOPT_CHECKSUM:
        case NETOPT_INTEGRITY_CHECK: {
            assert(max_len == sizeof(netopt_enable_t));
            /* mandatory for Enhanced ShockBurst */
            *((netopt_enable_t *)val) = NETOPT_ENABLE;
            return sizeof(netopt_enable_t);
        } break;
        case NETOPT_DEVICE_TYPE: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = NETDEV_TYPE_NRF24L01P_NG;
            return sizeof(uint16_t);
        } break;
        case NETOPT_PROTO: {
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)val) = NRF24L01P_NG_UPPER_LAYER_PROTOCOL;
            return sizeof(gnrc_nettype_t);
        } break;
        case NETOPT_MAX_PDU_SIZE: {
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = NRF24L01P_NG_MAX_PAYLOAD_WIDTH -
                                 NRF24L01P_NG_ADDR_WIDTH
                                 - 1;
            return sizeof(uint16_t);
        }
        case NETOPT_RETRANS: {
            assert(max_len == sizeof(uint8_t));
            *((uint8_t *)val) = nrf24l01p_ng_get_max_retransm(dev);
            return sizeof(uint8_t);
        }
        case NETOPT_RX_TIMEOUT: {
            assert(max_len == sizeof(uint32_t));
            *((uint32_t *)val) =
                (uint32_t)nrf24l01p_ng_get_retransm_delay(dev, NULL);
            return sizeof(uint32_t);
        } break;
        case NETOPT_STATE: {
            assert(max_len == sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _state_to_netif(dev->state);
            return sizeof(netopt_state_t);
        } break;
        case NETOPT_TX_POWER: {
            assert(max_len == sizeof(int16_t));
            *((int16_t *)val) = (int16_t)nrf24l01p_ng_get_tx_power(dev, NULL);
            return sizeof(uint16_t);
        } break;
        default:
            DEBUG("[nrf24l01p_ng] Unsupported netdev option %d\n", opt);
            return -ENOTSUP;
    }
}

/**
 * @brief   @ref netdev_driver_t::set
 *
 * @param[in] netdev            Abstract network device handle
 * @param[in] opt               netdev option type
 * @param[in] val               Option value
 * @param[in] len               Size of option value
 *
 * @return                      Size of written option value
 * @return                      negative number, on failure
 * @retval -ENOTSUP             Unsupported netdev option @p opt
 */
static
int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    nrf24l01p_ng_t *dev = (nrf24l01p_ng_t *)netdev;

    switch (opt) {
        case NETOPT_ADDRESS: {
            /* common address length for all pipes */
            assert(len == NRF24L01P_NG_ADDR_WIDTH);
            int ret = nrf24l01p_ng_set_rx_address(dev, val, NRF24L01P_NG_P0);
            return ret ? ret : (int)len;
        } break;
        case NETOPT_CHANNEL: {
            assert(len == sizeof(uint16_t));
            uint16_t ch = *((uint16_t *)val);
            int ret = nrf24l01p_ng_set_channel(dev, (uint8_t)ch);
            return ret ? ret : (int)sizeof(uint16_t);
        } break;
        case NETOPT_CHECKSUM:
        case NETOPT_INTEGRITY_CHECK: {
            assert(len == sizeof(netopt_enable_t));
            nrf24l01p_ng_crc_t crc =
                (*((netopt_enable_t *)val) == NETOPT_ENABLE)
                ? NRF24L01P_NG_CRC_2BYTE : NRF24L01P_NG_CRC_0BYTE;
            int ret = nrf24l01p_ng_set_crc(dev, crc);
            return ret ? ret : (int)sizeof(netopt_enable_t);
        } break;
        case NETOPT_RETRANS: {
            assert(len == sizeof(uint8_t));
            uint8_t n = *((uint8_t *)val);
            int ret = nrf24l01p_ng_set_max_retransm(dev, n);
            return ret ? ret : (int)sizeof(uint8_t);
        } break;
        case NETOPT_RX_TIMEOUT: {
            assert(len == sizeof(uint32_t));
            uint32_t us = *((uint32_t *)val);
            nrf24l01p_ng_ard_t ard = nrf24l01p_ng_valtoe_ard(us);
            int ret = nrf24l01p_ng_set_retransm_delay(dev, ard);
            return ret ? ret : (int)sizeof(uint32_t);
        } break;
        case NETOPT_STATE: {
            assert(len == sizeof(netopt_state_t));
            nrf24l01p_ng_state_t s =
                _state_from_netif(*((netopt_state_t *)val));
            int ret = nrf24l01p_ng_set_state(dev, s);
            return ret < 0 ? ret : (int)sizeof(netopt_state_t);
        } break;
        case NETOPT_TX_POWER: {
            assert(len == sizeof(int16_t));
            int16_t dbm = *((int16_t *)val);
            nrf24l01p_ng_tx_power_t txp = nrf24l01p_ng_valtoe_tx_power(dbm);
            int ret = nrf24l01p_ng_set_tx_power(dev, txp);
            return ret ? ret : (int)sizeof(int16_t);
        } break;
        default:
            DEBUG("[nrf24l01p_ng] Unsupported netdev option %d\n", opt);
            return -ENOTSUP;
    }
}
