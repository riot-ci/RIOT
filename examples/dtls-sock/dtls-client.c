/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       DTLS sock client example
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include <stdio.h>

#include "event/thread.h"
#include "event/timeout.h"
#include "net/sock/async/event.h"
#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/ipv6/addr.h"
#include "net/credman.h"

#include "tinydtls_keys.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define SOCK_DTLS_CLIENT_TAG (2)

#ifdef CONFIG_DTLS_ECC
static const ecdsa_public_key_t other_pubkeys[] = {
    { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
};

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_CLIENT_TAG,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key,
            .public_key = {
                .x = ecdsa_pub_key_x,
                .y = ecdsa_pub_key_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys,
            .client_keys_size = ARRAY_SIZE(other_pubkeys),
        }
    },
};

#else /* ifdef CONFIG_DTLS_PSK */
static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_CLIENT_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif

static void _timeout_handler(event_t *);

static uint8_t _recv_buf[512];
static sock_udp_t _udp_sock;
static sock_dtls_t _dtls_sock;
static event_timeout_t _timeouter;
static event_t _timeout = { .handler = _timeout_handler };

static void _timeout_handler(event_t *event)
{
    (void)event;
    puts("Session handshake timed out");
    sock_dtls_close(&_dtls_sock);
    sock_udp_close(&_udp_sock);
}

static void _dtls_handler(sock_dtls_t *sock, sock_async_flags_t type, void *arg)
{
    sock_dtls_session_t session = { 0 };

    event_timeout_clear(&_timeouter);
    if (type & SOCK_ASYNC_CONN_RECV) {
        char *send_data = arg;
        puts("Session handshake received");
        if (sock_dtls_recv(sock, &session, _recv_buf, sizeof(_recv_buf),
                           0) != -SOCK_DTLS_HANDSHAKE) {
            puts("Error creating session");
            sock_dtls_close(sock);
            sock_udp_close(&_udp_sock);
            return;
        }
        puts("Connection to server successful");
        printf("Sending data \"%s\"\n", send_data);
        if (sock_dtls_send(sock, &session, send_data, strlen(send_data),
                           0) < 0) {
            puts("Error sending data");
            sock_dtls_session_destroy(sock, &session);
            sock_dtls_close(sock);
            sock_udp_close(&_udp_sock);
        }
        else {
            printf("Sent DTLS message\n");
            event_timeout_set(&_timeouter, 5 * US_PER_SEC);
        }
    }
    else if (type & SOCK_ASYNC_CONN_FIN) {
        puts("Session was destroyed");
        sock_dtls_close(sock);
        sock_udp_close(&_udp_sock);
    }
    else if (type & SOCK_ASYNC_CONN_RDY) {
        puts("Session became ready");
    }
    else if (type & SOCK_ASYNC_MSG_RECV) {
        int res;

        if ((res = sock_dtls_recv(sock, &session, _recv_buf, sizeof(_recv_buf),
                                  0)) < 0) {
            printf("Error receiving DTLS message\n");
        }
        else {
            printf("Received %d bytes: \"%.*s\"\n", (int)res, (int)res,
                   (char *)_recv_buf);
        }
        puts("Terminating session");
        sock_dtls_session_destroy(sock, &session);
        sock_dtls_close(sock);
        sock_udp_close(&_udp_sock);
    }
    else if (type & SOCK_ASYNC_MSG_SENT) {
        puts("DTLS message was sent");
    }
    else if (type & SOCK_ASYNC_PATH_PROP) {
        puts("Path property changed");
    }
}

static int client_send(char *addr_str, char *data)
{
    ssize_t res;
    sock_dtls_session_t session;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = 12345;
    remote.port = DTLS_DEFAULT_PORT;

    event_timeout_init(&_timeouter, &event_queue_medium, &_timeout);
    /* get interface */
    char* iface = ipv6_addr_split_iface(addr_str);
    if (iface) {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("Invalid network interface");
            return -1;
        }
        remote.netif = pid;
    } else if (gnrc_netif_numof() == 1) {
        /* assign the single interface found in gnrc_netif_numof() */
        remote.netif = gnrc_netif_iter(NULL)->pid;
    } else {
        /* no interface is given, or given interface is invalid */
        /* FIXME This probably is not valid with multiple interfaces */
        remote.netif = SOCK_ADDR_ANY_NETIF;
    }

    if (!ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, addr_str)) {
        puts("Error parsing destination address");
        return -1;
    }

    if (sock_udp_create(&_udp_sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return -1;
    }

    if (sock_dtls_create(&_dtls_sock, &_udp_sock,
                         SOCK_DTLS_CLIENT_TAG,
                         SOCK_DTLS_1_2, SOCK_DTLS_CLIENT) < 0) {
        puts("Error creating DTLS sock");
        sock_udp_close(&_udp_sock);
        return -1;
    }

    sock_dtls_event_init(&_dtls_sock, &event_queue_medium, _dtls_handler,
                         data);
    res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %d\n", (int)res);
        sock_dtls_close(&_dtls_sock);
        sock_udp_close(&_udp_sock);
        return -1;
    }

    res = sock_dtls_session_init(&_dtls_sock, &remote, &session);
    if (res <= 0) {
        sock_dtls_close(&_dtls_sock);
        sock_udp_close(&_udp_sock);
        return res;
    }

    event_timeout_set(&_timeouter, 5 * US_PER_SEC);
    return 0;
}

int dtls_client_cmd(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage %s <addr> <data>\n", argv[0]);
        return 1;
    }

    return client_send(argv[1], argv[2]);
}
