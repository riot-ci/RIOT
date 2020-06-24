/*
 * Copyright (C) 2019  Javier FILEIV <javier.fileiv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       MQTT common RIOT interface functions
 *
 * @author      Javier FILEIV <javier.fileiv@gmail.com>
 */
#include <string.h>
#include <errno.h>

#ifdef SOCK_HAS_IPV6
#include <net/ipv6/addr.h>
#endif

#include <net/ipv4/addr.h>
#include <net/af.h>
#include <net/sock.h>
#include <net/sock/tcp.h>
#include <mqtt.h>
#include "MQTTClient.h"
#include <xtimer.h>
#include <tsrb.h>
#include <log.h>

#define ENABLE_DEBUG                        (0)
#include "debug.h"

#define IP_MAX_LEN_ADDRESS                 (39)
#ifdef MODULE_LWIP
#define TSRB_MAX_SIZE                       (1024)
static uint8_t buffer[TSRB_MAX_SIZE];
static tsrb_t tsrb_lwip_tcp;
#endif

#ifndef PAHO_MQTT_YIELD_MS
#define PAHO_MQTT_YIELD_MS                  (10)
#endif

static int mqtt_read(struct Network *n, unsigned char *buf, int len,
                     int timeout_ms)
{
    int _timeout;
    int _len;
    void *temp_buf;
    int rc = -1;

#ifdef MODULE_LWIP
    /* As LWIP doesn't support reads packets byte per byte and
     * PAHO MQTT reads like that to decode it on the fly,
     * we read TSRB_MAX_SIZE at once and keep them in a ring buffer.
     */
    uint8_t _internal_buffer[TSRB_MAX_SIZE];
    temp_buf = _internal_buffer;
    _len = TSRB_MAX_SIZE;
    _timeout = 0;
#else
    temp_buf = buf;
    _len = len;
    _timeout = timeout_ms;
#endif

    uint64_t send_tick = xtimer_now64().ticks64 + xtimer_ticks_from_usec64(timeout_ms * US_PER_MS).ticks64;
    do {
        rc = sock_tcp_read(&n->sock, temp_buf, _len, _timeout);
        if (rc == -EAGAIN) {
            rc = 0;
        }

    #ifdef MODULE_LWIP
        if (rc > 0) {
            tsrb_add(&tsrb_lwip_tcp, temp_buf, rc);
        }

        rc = tsrb_get(&tsrb_lwip_tcp, buf, len);
    #endif
    } while (rc < len && xtimer_now64().ticks64 < send_tick && rc >= 0);

#ifdef ENABLE_DEBUG
#ifdef MODULE_LWIP
    if (rc > 0) {
        DEBUG("MQTT buf asked for %d, available to read %d\n", rc, tsrb_avail(&tsrb_lwip_tcp));
        for (int i = 0; i < rc; i++) {
            DEBUG("0x%02X ", buf[i]);
        }
        DEBUG("\n");
    }
#endif
#endif

    return rc;
}

static int mqtt_write(struct Network *n, unsigned char *buf, int len,
                      int timeout_ms)
{
    /* timeout is controlled by upper layer in PAHO */
    (void) timeout_ms;

    return sock_tcp_write(&n->sock, buf, len);
}

void NetworkInit(Network *n)
{
#ifdef MODULE_LWIP
    tsrb_init(&tsrb_lwip_tcp, buffer, TSRB_MAX_SIZE);
#endif
    n->mqttread = mqtt_read;
    n->mqttwrite = mqtt_write;
}

int NetworkConnect(Network *n, char *addr_ip, int port)
{
    int ret =-1;
    sock_tcp_ep_t *remote = NULL;
    char _local_ip[IP_MAX_LEN_ADDRESS];
    /* ipvN_addr_from_str modifies input buffer */
    strncpy(_local_ip, addr_ip, sizeof(_local_ip));


    sock_tcp_ep_t remote4 = SOCK_IPV4_EP_ANY;
    if (ipv4_addr_from_str((ipv4_addr_t *)&remote4.addr,
                           _local_ip) == NULL) {
#ifdef SOCK_HAS_IPV6
        LOG_INFO("Error IPv4: unable to parse destination address, trying with IPv6\n");
        sock_tcp_ep_t remote6 = SOCK_IPV6_EP_ANY;
        if (ipv6_addr_from_str((ipv6_addr_t *)&remote6.addr, _local_ip) == NULL) {
            LOG_ERROR("Error IPv6: unable to parse destination address\n");
            return ret;
        }
        remote6.port = port;
        remote = &remote6;
        goto connect;
#else
        LOG_ERROR("Error IPv4: unable to parse destination address\n");
#endif
        return ret;
    }
    remote4.port = port;
    remote = &remote4;
#ifdef SOCK_HAS_IPV6
connect:
#endif
    ret = sock_tcp_connect(&n->sock, remote, 0, 0);
    if (ret < 0) {
        LOG_ERROR("paho-mqtt: unable to connect (%d)\n", ret);
    } else {
        ret = 0;
    }
    return ret;
}

void NetworkDisconnect(Network *n)
{
    sock_tcp_disconnect(&n->sock);
}

void TimerInit(Timer *timer)
{
    timer->set_ticks.ticks64 = 0;
    timer->ticks_timeout.ticks64 = 0;
}

char TimerIsExpired(Timer *timer)
{
    return (TimerLeftMS(timer) == 0);
}

void TimerCountdownMS(Timer *timer, unsigned int timeout_ms)
{
    timer->set_ticks = xtimer_now64();
    timer->ticks_timeout = xtimer_ticks_from_usec64(timeout_ms * US_PER_MS);
}

void TimerCountdown(Timer *timer, unsigned int timeout_s)
{
    TimerCountdownMS(timer, timeout_s * MS_PER_SEC);
}

int TimerLeftMS(Timer *timer)
{
    xtimer_ticks64_t diff_ticks = xtimer_diff64(xtimer_now64(), timer->set_ticks);  /* should be always greater than 0 */
    if (xtimer_less64(diff_ticks ,timer->ticks_timeout)) {
        return (xtimer_usec_from_ticks64(xtimer_diff64(timer->ticks_timeout, diff_ticks)) / US_PER_MS);
    }
    return 0;
}

void MutexInit(Mutex *mutex)
{
    mutex_init(&mutex->lock);
}

int MutexLock(Mutex *mutex)
{
    mutex_lock(&mutex->lock);
    return 0;
}

int MutexUnlock(Mutex *mutex)
{
    mutex_unlock(&mutex->lock);
    return 0;
}

void *mqtt_riot_run(void *arg)
{
    MQTTClient *client = (MQTTClient *)arg;
    assert(client);

    while (1) {
        int rc;
        MutexLock(&client->mutex);
        if ((rc = MQTTYield(client, PAHO_MQTT_YIELD_MS)) != 0) {
            LOG_DEBUG("riot_iface: error while MQTTYield()(%d)\n", rc);
        }
        MutexUnlock(&client->mutex);
        /* let other threads do their work */
        xtimer_usleep(MQTT_YIELD_POLLING_MS * US_PER_MS);
    }
    return NULL;
}

int ThreadStart(Thread *thread, void (*fn)(void *), void *arg)
{
    (void) fn;
    thread->pid = thread_create(thread->stack, sizeof(thread->stack),
                                MQTT_THREAD_PRIORITY,
                                THREAD_CREATE_STACKTEST, mqtt_riot_run, arg,
                                "paho_mqtt_riot");
    return thread->pid;
}
