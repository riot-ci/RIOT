/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ethocan
 * @{
 *
 * @file
 * @brief       Implementation of an Ethernet-over-CAN driver
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <string.h>

#include "ethocan.h"
#include "luid.h"
#include "random.h"

#include "net/netdev/eth.h"
#include "net/eui64.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static uint16_t crc16_update(uint16_t crc, uint8_t octet);
static uint8_t state_blocked(ethocan_t *ctx, uint8_t old_state);
static uint8_t state_recv(ethocan_t *ctx, uint8_t old_state);
static uint8_t state_send(ethocan_t *ctx, uint8_t old_state);
static uint8_t state(ethocan_t *ctx, uint8_t src);
static void _isr_uart(void *arg, uint8_t c);
static void _isr_gpio(void *arg);
static void _isr_xtimer(void *arg);
static void clear_recv_buf(ethocan_t *ctx);
static void _isr(netdev_t *netdev);
static int _recv(netdev_t *dev, void *buf, size_t len, void *info);
static uint8_t wait_for_state(ethocan_t *ctx, uint8_t state);
static int send_octet(ethocan_t *ctx, uint8_t c);
static int _send(netdev_t *dev, const iolist_t *iolist);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _init(netdev_t *dev);
void ethocan_setup(ethocan_t *ctx, const ethocan_params_t *params);

static inline void flag_set(ethocan_t *ctx, uint8_t flag)
{
    ctx->flags |= flag;
}

static inline void flag_clear(ethocan_t *ctx, uint8_t flag)
{
    ctx->flags &= ~flag;
}

static inline int flag_isset(ethocan_t *ctx, uint8_t flag)
{
    return (ctx->flags & flag) ? 1 : 0;
}

static uint16_t crc16_update(uint16_t crc, uint8_t octet)
{
    crc = (uint8_t)(crc >> 8) | (crc << 8);
    crc ^= octet;
    crc ^= (uint8_t)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
    return crc;
}

static uint8_t state_blocked(ethocan_t *ctx, uint8_t old_state)
{
    /* When we left the RECV state, user land has to look if this frame
     * should be processed. By queuing NETDEV_EVENT_ISR, the netif thread
     * will call _isr at some time. But nobody knows when ... */
    if (old_state == ETHOCAN_STATE_RECV) {
        flag_set(ctx, ETHOCAN_FLAG_RECV_BUF_DIRTY);
        flag_clear(ctx, ETHOCAN_FLAG_ESC_RECEIVED);
        ctx->netdev.event_callback((netdev_t *) ctx, NETDEV_EVENT_ISR);
    }

    /* Enable GPIO interrupt for listing to
     * the falling edge of the start bit */
    gpio_irq_enable(ctx->sense_pin);

    /* The timeout will bring us back into IDLE state by a random time
     * between 0 and the default timeout. Thus, we will block sending
     * frames for a certain time and wait for incoming frames */
    uint32_t backoff = random_uint32_range(ctx->timeout_ticks / 10, ctx->timeout_ticks);
    xtimer_set(&ctx->timeout, backoff);

    return ETHOCAN_STATE_BLOCKED;
}

static uint8_t state_recv(ethocan_t *ctx, uint8_t old_state)
{
    uint8_t next_state = ETHOCAN_STATE_RECV;

    if (old_state != ETHOCAN_STATE_RECV) {
        /* We freshly entered this state due to a GPIO interrupt.
         * Thus we detected the falling edge of the start bit.
         * Disable GPIO IRQs during the transmission. */
        gpio_irq_disable(ctx->sense_pin);
    }
    else {
        /* Reentered this state -> a new char has been received from UART.
         * Handle ESC and END octect ... */
        int esc = flag_isset(ctx, ETHOCAN_FLAG_ESC_RECEIVED);
        if (!esc && ctx->uart_octect == ETHOCAN_OCTECT_ESC) {
            flag_set(ctx, ETHOCAN_FLAG_ESC_RECEIVED);
        }
        else if (!esc && ctx->uart_octect == ETHOCAN_OCTECT_END) {
            flag_set(ctx, ETHOCAN_FLAG_END_RECEIVED);
            next_state = ETHOCAN_STATE_BLOCKED;
        }
        else {
            if (esc) {
                flag_clear(ctx, ETHOCAN_FLAG_ESC_RECEIVED);
            }
            /* Since the dirty flag is set after the RECV state is left,
             * it indicates that the receive buffer contains unprocessed data
             * from a previos received frame.
             * Thus, we just ignore new data. */
            if (!flag_isset(ctx, ETHOCAN_FLAG_RECV_BUF_DIRTY)
                && ctx->recv_buf_ptr < ETHOCAN_FRAME_LEN) {
                ctx->recv_buf[ctx->recv_buf_ptr++] = ctx->uart_octect;
            }
        }
    }

    if (next_state == ETHOCAN_STATE_RECV) {
        /* Start the timeout timer if we are staying in RECV state. */
        xtimer_set(&ctx->timeout, ctx->timeout_ticks);
    }

    return next_state;
}

static uint8_t state_send(ethocan_t *ctx, uint8_t old_state)
{
    if (old_state != ETHOCAN_STATE_SEND) {
        /* Disable GPIO IRQs during the transmission. */
        gpio_irq_disable(ctx->sense_pin);
    }

    /* Don't trance any END octets ... the timeout or the END signal
     * will bring us back to the BLOCKED state after _send has emitted
     * it's last octect. */

    xtimer_set(&ctx->timeout, ctx->timeout_ticks);

    return ETHOCAN_STATE_SEND;
}

static uint8_t state(ethocan_t *ctx, uint8_t src)
{
    /* Make sure no other thread or ISR interrupts state transitions */
    int irq_state = irq_disable();

    uint8_t old_state = ctx->state;
    uint8_t new_state = ETHOCAN_STATE_UNDEF;

    /* State change based on the input signal */
    switch (old_state) {
        case ETHOCAN_STATE_UNDEF:
            if (src == ETHOCAN_SIGNAL_INIT) {
                new_state = ETHOCAN_STATE_BLOCKED;
            }
            break;
        case ETHOCAN_STATE_BLOCKED:
            if (src == ETHOCAN_SIGNAL_GPIO) {
                new_state = ETHOCAN_STATE_RECV;
            }
            if (src == ETHOCAN_SIGNAL_XTIMER) {
                new_state = ETHOCAN_STATE_IDLE;
            }
            break;
        case ETHOCAN_STATE_IDLE:
            if (src == ETHOCAN_SIGNAL_GPIO) {
                new_state = ETHOCAN_STATE_RECV;
            }
            if (src == ETHOCAN_SIGNAL_SEND) {
                new_state = ETHOCAN_STATE_SEND;
            }
            break;
        case ETHOCAN_STATE_RECV:
            if (src == ETHOCAN_SIGNAL_UART) {
                new_state = ETHOCAN_STATE_RECV;
            }
            if (src == ETHOCAN_SIGNAL_XTIMER) {
                new_state = ETHOCAN_STATE_BLOCKED;
            }
            break;
        case ETHOCAN_STATE_SEND:
            if (src == ETHOCAN_SIGNAL_UART) {
                new_state = ETHOCAN_STATE_SEND;
            }
            if (src == ETHOCAN_SIGNAL_XTIMER) {
                new_state = ETHOCAN_STATE_BLOCKED;
            }
            if (src == ETHOCAN_SIGNAL_END) {
                new_state = ETHOCAN_STATE_BLOCKED;
            }
            break;
    }

    /* Invalid signal in current state */
    if (new_state < ETHOCAN_STATE_BLOCKED || new_state > ETHOCAN_STATE_SEND) {
        goto exit;
    }

    /* Call state specific function */
    while (1) {
        uint8_t next_state = new_state;
        switch (next_state) {
            case ETHOCAN_STATE_BLOCKED: next_state = state_blocked(ctx, old_state); break;
            case ETHOCAN_STATE_RECV: next_state = state_recv(ctx, old_state); break;
            case ETHOCAN_STATE_SEND: next_state = state_send(ctx, old_state); break;
        }

        if (next_state == new_state) {
            /* No state change occured within the state's function */
            break;
        }
        else {
            old_state = new_state;
            new_state = next_state;
        }
    }

    /* Invalid state has been set in state functions */
    if (new_state < ETHOCAN_STATE_BLOCKED || new_state > ETHOCAN_STATE_SEND) {
        goto exit;
    }

    ctx->state = new_state;
    mutex_unlock(&ctx->state_mtx);

exit:
    irq_restore(irq_state);
    return ctx->state;
}

static void _isr_uart(void *arg, uint8_t c)
{
    ethocan_t *dev = (ethocan_t *) arg;

    dev->uart_octect = c;
    state(dev, ETHOCAN_SIGNAL_UART);
}

static void _isr_gpio(void *arg)
{
    ethocan_t *dev = (ethocan_t *) arg;

    state(dev, ETHOCAN_SIGNAL_GPIO);
}

static void _isr_xtimer(void *arg)
{
    ethocan_t *dev = (ethocan_t *) arg;

    state(dev, ETHOCAN_SIGNAL_XTIMER);
}

static void clear_recv_buf(ethocan_t *ctx)
{
    int irq_state = irq_disable();

    ctx->recv_buf_ptr = 0;
    flag_clear(ctx, ETHOCAN_FLAG_RECV_BUF_DIRTY);
    flag_clear(ctx, ETHOCAN_FLAG_END_RECEIVED);
    flag_clear(ctx, ETHOCAN_FLAG_ESC_RECEIVED);
    irq_restore(irq_state);
}

static void _isr(netdev_t *netdev)
{
    ethocan_t *ctx = (ethocan_t *) netdev;

    /* Get current flags */
    int irq_state = irq_disable();
    int dirty = flag_isset(ctx, ETHOCAN_FLAG_RECV_BUF_DIRTY);
    int end = flag_isset(ctx, ETHOCAN_FLAG_END_RECEIVED);

    irq_restore(irq_state);

    /* If the recveive buffer does not contain any data just abort ... */
    if (!dirty) {
        DEBUG("ethocan _isr(): no frame -> drop\n");
        return;
    }

    /* If we havn't received a valid END octect just drop the incomplete frame. */
    if (!end) {
        DEBUG("ethocan _isr(): incomplete frame -> drop\n");
        clear_recv_buf(ctx);
        return;
    }

    /* The set dirty flag prevents recv_buf or recv_buf_ptr from being
     * touched in ISR context. Thus, it is safe to work with them without
     * IRQs being disabled or mutexted being locked. */

    /* This frame is not for us ... just drop it */
    ethernet_hdr_t *hdr = (ethernet_hdr_t *) ctx->recv_buf;
    if ((hdr->dst[0] & 0x1) == 0 && memcmp(hdr->dst, ctx->mac_addr, ETHERNET_ADDR_LEN) != 0) {
        DEBUG("ethocan _isr(): dst mac not matching frame -> drop\n");
        clear_recv_buf(ctx);
        return;
    }

    /* Check the CRC */
    uint16_t crc = 0xffff;
    for (size_t i = 0; i < ctx->recv_buf_ptr; i++) {
        crc = crc16_update(crc, ctx->recv_buf[i]);
    }
    if (crc != 0x0000) {
        DEBUG("ethocan _isr(): wrong crc 0x%04x -> drop\n", crc);
        clear_recv_buf(ctx);
        return;
    }

    /* Finally schedule a _recv method call */
    DEBUG("ethocan _isr(): NETDEV_EVENT_RX_COMPLETE\n");
    ctx->netdev.event_callback((netdev_t *) ctx, NETDEV_EVENT_RX_COMPLETE);
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    ethocan_t *ctx = (ethocan_t *) dev;

    (void)info;

    size_t pktlen = ctx->recv_buf_ptr;
    if (!buf && !len) {
        /* Return the amount of received bytes */
        return pktlen;
    }
    else if (!buf && len) {
        /* The user drops the packet */
        clear_recv_buf(ctx);
        return pktlen;
    }
    else if (len < pktlen) {
        /* The provided buffer is too small! */
        DEBUG("ethocan _recv(): receive buffer too small\n");
        clear_recv_buf(ctx);
        return -1;
    }
    else {
        /* Copy the packet to the provided buffer. */
        memcpy(buf, ctx->recv_buf, pktlen);
        clear_recv_buf(ctx);
        return pktlen;
    }
}

static uint8_t wait_for_state(ethocan_t *ctx, uint8_t state)
{
    do {
        /* This mutex is unlocked by the state machine
         * after every state transistion */
        mutex_lock(&ctx->state_mtx);
    } while (state != ETHOCAN_STATE_ANY && ctx->state != state);
    return ctx->state;
}

static int send_octet(ethocan_t *ctx, uint8_t c)
{
    uart_write(ctx->uart, (uint8_t *) &c, 1);

    /* Wait for a state transistion */
    uint8_t state = wait_for_state(ctx, ETHOCAN_STATE_ANY);
    if (state != ETHOCAN_STATE_SEND) {
        /* Timeout */
        DEBUG("ethocan send_octet(): timeout\n");
        return -2;
    }
    else if (ctx->uart_octect != c) {
        /* Missmatch */
        DEBUG("ethocan send_octet(): missmatch\n");
        return -1;
    }

    return 0;
}

static int _send(netdev_t *dev, const iolist_t *iolist)
{
    ethocan_t *ctx = (ethocan_t *) dev;
    int8_t retries = 3;
    size_t pktlen;
    uint16_t crc;

send:
    crc = 0xffff;
    pktlen = 0;

    /* Wait until we are in state IDLE */
    wait_for_state(ctx, ETHOCAN_STATE_IDLE);

    /* Switch to state SEND */
    state(ctx, ETHOCAN_SIGNAL_SEND);
    wait_for_state(ctx, ETHOCAN_STATE_SEND);

    /* Send packet buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        size_t n = iol->iol_len;
        pktlen += n;
        uint8_t *ptr = iol->iol_base;
        while (n--) {
            /* Escape special octets */
            if (*ptr == ETHOCAN_OCTECT_ESC || *ptr == ETHOCAN_OCTECT_END) {
                if (send_octet(ctx, ETHOCAN_OCTECT_ESC)) {
                    goto collision;
                }
            }

            /* Send data octet */
            if (send_octet(ctx, *ptr)) {
                goto collision;
            }

            /* Update CRC */
            crc = crc16_update(crc, *ptr);

            ptr++;
        }
    }

    /* Send CRC */
    network_uint16_t crc_nw = byteorder_htons(crc);
    if (send_octet(ctx, crc_nw.u8[0])) {
        goto collision;
    }
    if (send_octet(ctx, crc_nw.u8[1])) {
        goto collision;
    }

    /* Send END octet */
    if (send_octet(ctx, ETHOCAN_OCTECT_END)) {
        goto collision;
    }

    /* Get out of the SEND state */
    state(ctx, ETHOCAN_SIGNAL_END);

    return pktlen;

collision:
    DEBUG("ethocan _send(): collision!\n");
    if (--retries < 0) {
        return 0;
    }
    goto send;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                memcpy(value, ((ethocan_t *) dev)->mac_addr, ETHERNET_ADDR_LEN);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        default:
            res = netdev_eth_get(dev, opt, value, max_len);
            break;
    }

    return res;
}

static int _init(netdev_t *dev)
{
    ethocan_t *ctx = (ethocan_t *) dev;

    /* Set state machine to defaults */
    int irq_state = irq_disable();

    ctx->recv_buf_ptr = 0;
    ctx->flags = 0;
    ctx->state = ETHOCAN_STATE_UNDEF;
    irq_restore(irq_state);

    state(ctx, ETHOCAN_SIGNAL_INIT);

    return 0;
}

static const netdev_driver_t netdev_driver_ethocan = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = netdev_eth_set
};

void ethocan_setup(ethocan_t *ctx, const ethocan_params_t *params)
{
    ctx->netdev.driver = &netdev_driver_ethocan;

    ctx->state = ETHOCAN_STATE_UNDEF;
    mutex_init(&ctx->state_mtx);

    ctx->uart = params->uart;
    uart_init(ctx->uart, params->baudrate, _isr_uart, (void *) ctx);

    ctx->sense_pin = params->sense_pin;
    gpio_init_int(ctx->sense_pin, GPIO_IN, GPIO_FALLING, _isr_gpio, (void *) ctx);
    gpio_irq_disable(ctx->sense_pin);

    luid_get(ctx->mac_addr, ETHERNET_ADDR_LEN);
    ctx->mac_addr[0] &= (0x2);
    ctx->mac_addr[0] &= ~(0x1);
    DEBUG("ethocan ethocan_setup(): mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
          ctx->mac_addr[0], ctx->mac_addr[1], ctx->mac_addr[2],
          ctx->mac_addr[3], ctx->mac_addr[4], ctx->mac_addr[5]
          );

    ctx->timeout_ticks = xtimer_ticks_from_usec(ETHOCAN_TIMEOUT_USEC).ticks32;
    ctx->timeout.callback = _isr_xtimer;
    ctx->timeout.arg = ctx;
}
