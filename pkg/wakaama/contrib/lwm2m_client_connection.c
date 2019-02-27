/*******************************************************************************
*
* Copyright (c) 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Christian Renz - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/

/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     lwm2m_client
 *
 * @file
 * @brief       Connection handle for LWM2M client implementation using Wakaama
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <stddef.h>
#include "net/lwm2m.h"
#include "net/netif.h"

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "lwm2m_client_connection.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define URI_LENGTH 256

/**
 * @brief Creates a new connection object based on the security instance
 *        represented by @p instance_id.
 *
 * @param[in] instance_id ID number of the instance of security object
 * @param[in, out] client_data LWM2M client data
 *
 * @return Pointer to the new connection
 */
static lwm2m_client_connection_t *_connection_create(int instance_id,
                                        lwm2m_client_data_t *client_data);

/**
 * @brief Gets the URI from an @p instance_id of a @p obj security object
 *
 * @param[in] obj security object
 * @param[in] instance_id ID number of the instance of security object
 * @param[out] uri_buffer buffer to place the URI
 * @param[in] buffer_size size of @p uri_buffer
 *
 * @return Pointer to the URI in success
 * @return NULL otherwise
 */
static char *_get_uri_from_security_obj(lwm2m_object_t *obj, int instance_id,
                           char *uri_buffer, int buffer_size);

/**
 * @brief Sends data with a specified connection @p conn
 *
 * @param[in] conn connection to use to send data
 * @param[in] buffer data to send
 * @param[in] buffer_size size of @p buffer
 * @param[in] client_data LWM2M client data
 *
 * @return 0 on success
 * @return -1 otherwise
 */
static int _connection_send(lwm2m_client_connection_t *conn, uint8_t *buffer,
                            size_t buffer_size,
                            lwm2m_client_data_t *client_data);


void *lwm2m_connect_server(uint16_t sec_obj_inst_id, void *user_data)
{
    lwm2m_client_data_t *client_data = (lwm2m_client_data_t *)user_data;
    lwm2m_list_t *instance;
    lwm2m_client_connection_t *new_conn = NULL;

    /* get the security object instance */
    instance = LWM2M_LIST_FIND(client_data->obj_security->instanceList,
                               sec_obj_inst_id);
    if (instance == NULL) {
        DEBUG("[lwm2m_connect_server] Could not find sec object instance\n");
        return NULL;
    }

    new_conn = _connection_create(instance->id, client_data);
    if (new_conn) {
        DEBUG("[lwm2m_connect_server] Connection created\n");
        /* if the connections list is empty this is the first node, if not
         * attach to the last one */
        if (!client_data->conn_list) {
            client_data->conn_list = new_conn;
        }
        else {
            lwm2m_client_connection_t *last = client_data->conn_list;
            while (last->next != NULL) {
                last = last->next;
            }
            last->next = new_conn;
        }
    }

    return new_conn;
}

void lwm2m_close_connection(void *sessionH, void *user_data)
{
    lwm2m_client_connection_t *conn = (lwm2m_client_connection_t *) sessionH;
    lwm2m_client_data_t *client_data = (lwm2m_client_data_t *) user_data;

    if (conn == client_data->conn_list) {
        client_data->conn_list = conn->next;
    }
    else {
        lwm2m_client_connection_t *prev = client_data->conn_list;

        while(prev != NULL && prev->next != conn) {
            prev = prev->next;
        }
        if (prev != NULL) {
            prev->next = conn->next;
            lwm2m_free(conn);
        }
    }
}

bool lwm2m_session_is_equal(void *session1, void *session2, void *user_data)
{
    (void)user_data;
    lwm2m_client_connection_t *conn_1 = (lwm2m_client_connection_t *)session1;
    lwm2m_client_connection_t *conn_2 = (lwm2m_client_connection_t *)session2;

    return ((conn_1->remote.port == conn_2->remote.port) &&
            ipv6_addr_equal((ipv6_addr_t *)&(conn_1->remote.addr.ipv6),
                            (ipv6_addr_t *)&(conn_2->remote.addr.ipv6)));
}

uint8_t lwm2m_buffer_send(void *sessionH, uint8_t *buffer, size_t length,
                          void *userdata)
{
    lwm2m_client_data_t *client_data = (lwm2m_client_data_t *)userdata;
    lwm2m_client_connection_t *conn = (lwm2m_client_connection_t *)sessionH;

    if (!conn) {
        DEBUG("[lwm2m_buffer_send] Failed to send, missing connection\n");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    if (_connection_send(conn, buffer, length, client_data)) {
        DEBUG("[lwm2m_buffer_send] Failed to send\n");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    return COAP_NO_ERROR;
}

lwm2m_client_connection_t *lwm2m_client_connection_find(
                                lwm2m_client_connection_t *conn_list,
                                const sock_udp_ep_t *remote)
{
    lwm2m_client_connection_t *conn = conn_list;

    char ip[128];
    uint8_t ip_len = 128;

    ipv6_addr_to_str(ip, (ipv6_addr_t *)&remote->addr.ipv6, ip_len);
    DEBUG("Looking for connection from [%s]:%d\n", ip, remote->port);

    if (conn_list == NULL) {
        DEBUG("Conn list is null!");
    }

    while(conn != NULL) {
        ipv6_addr_to_str(ip, (ipv6_addr_t *)&conn->remote.addr.ipv6, ip_len);
        DEBUG("Comparing to [%s]:%d\n", ip, conn->remote.port);
        if ((conn->remote.port == remote->port) &&
            ipv6_addr_equal((ipv6_addr_t *)&(conn->remote.addr.ipv6),
                            (ipv6_addr_t *)&(remote->addr.ipv6))) {
            break;
        }
        conn = conn->next;
    }
    return conn;
}

int lwm2m_connection_handle_packet(lwm2m_client_connection_t *conn,
                                   uint8_t *buffer, size_t num_bytes,
                                   lwm2m_client_data_t *client_data)
{
    lwm2m_handle_packet(client_data->lwm2m_ctx, buffer, num_bytes, conn);
    return 0;
}

static int _connection_send(lwm2m_client_connection_t *conn, uint8_t *buffer,
                            size_t buffer_size,
                            lwm2m_client_data_t *client_data)
{
    ssize_t sent_bytes = sock_udp_send(&(client_data->sock), buffer,
                                       buffer_size, &(conn->remote));
    if (sent_bytes <= 0) {
        DEBUG("[_connection_send] Could not send UDP packet: %d\n", sent_bytes);
        return -1;
    }
    conn->last_send = lwm2m_gettime();
    return 0;
}

static lwm2m_client_connection_t *_connection_create(int instance_id,
                                        lwm2m_client_data_t *client_data)
{
    lwm2m_client_connection_t *conn = NULL;
    char *default_port;
    char *host;
    int iface;
    char *port;
    char *tmp;
    char *uri;
    char uri_buf[URI_LENGTH + 1];

    memset(uri_buf, 0, sizeof(uri_buf));
    DEBUG("Creating connection\n");
    /* get the server URI from the requested instance */
    uri = _get_uri_from_security_obj(client_data->obj_security, instance_id,
                                     uri_buf, sizeof(uri_buf) - 1);

    if (!uri) {
        DEBUG("[_connection_create] Could not get URI of instance\n");
        goto out;
    }

    /* parse the URI in the form "coaps://[host]:port" */
    if (!strncmp(uri, SCHEME_COAPS, strlen(SCHEME_COAPS))) {
        host = uri + strlen(SCHEME_COAPS);
        default_port = LWM2M_DTLS_PORT;
    }
    else if (!strncmp(uri, SCHEME_COAP, strlen(SCHEME_COAP))) {
        host = uri + strlen(SCHEME_COAP);
        default_port = LWM2M_STANDARD_PORT;
    }
    else {
        DEBUG("[_connection_create] Invalid protocol in server URI\n");
        goto out;
    }

    tmp = host;
    if (tmp[0] == '[') {
        host++;
        tmp = strrchr(tmp, ']');
    }

    port = strrchr(tmp, ':');
    if (!port) {
        *tmp = '\0';
        DEBUG("[_connection_create] No port specified, using default\n");
        port = default_port;
    }
    else {
        *(port - 1) = '\0';
        port++;
    }
    DEBUG("[_connection_create] Creating connection to Host: %s, Port: %s",
          host, port);

    /* allocate new connection */
    conn = lwm2m_malloc(sizeof(lwm2m_client_connection_t));
    if (!conn) {
        DEBUG("[_connection_create] Could not allocate new connection\n");
        goto out;
    }
    conn->next = client_data->conn_list;

    conn->remote.port = atoi(port);
    /* configure to any IPv6 */
    conn->remote.family = AF_INET6;
    conn->remote.netif = SOCK_ADDR_ANY_NETIF;

    if (!ipv6_addr_from_str((ipv6_addr_t *)&conn->remote.addr.ipv6, host)) {
        DEBUG("[_connection_create] IPv6 address malformed\n");
        goto free_out;
    }

    if (ipv6_addr_is_unspecified((const ipv6_addr_t *)&conn->remote.addr.ipv6)) {
        DEBUG("[_connection_create] Invalid server address ([::])\n");
        goto free_out;
    }

    /* If the address is a link-local one first check if interface is specified,
     * if not, check the number of interfaces and default to the first if there
     * is only one defined. */
    if (ipv6_addr_is_link_local((ipv6_addr_t *)&conn->remote.addr.ipv6)) {
        iface = ipv6_addr_split_iface(host);
        if (iface < 0) {
            /* get the number of net interfaces */
            unsigned netif_numof = 0;
            netif_t netif = NETIF_INVALID;
            while ((netif = netif_iter(netif)) != NETIF_INVALID) {
                    netif_numof++;
            }

            if (netif_numof == 1) {
                iface = netif_iter(NETIF_INVALID);
            }
            else {
                DEBUG("[_connection_create] No iface for link-local address\n");
                goto free_out;
            }
        }
        conn->remote.netif = iface;
    }

    conn->last_send = lwm2m_gettime();
    goto out;

free_out:
    lwm2m_free(conn);
    conn = NULL;
out:
    return conn;
}

static char *_get_uri_from_security_obj(lwm2m_object_t *obj, int instance_id,
                           char *uri_buffer, int buffer_size)
{
    int size = 1;
    char *res = NULL;

    /* allocate a data instance */
    lwm2m_data_t *data = lwm2m_data_new(size);

    /* get the uri from the security object */
    data->id = 0;
    obj->readFunc(instance_id, &size, &data, obj);

    if (data != NULL && data->type == LWM2M_TYPE_STRING &&
        data->value.asBuffer.length > 0) {
        if ((size_t)buffer_size > data->value.asBuffer.length) {
            strncpy(uri_buffer, (char *)data->value.asBuffer.buffer,
                    data->value.asBuffer.length);
            res = uri_buffer;
        }
    }

    lwm2m_data_free(size, data);
    return res;
}
