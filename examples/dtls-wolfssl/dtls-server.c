/*
 * Copyright (C) 2019 Daniele Lacamera
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
 * @brief       Demonstrating DTLS 1.2 server using wolfSSL
 *
 * @author      Daniele Lacamera <daniele@wolfssl.com>
 * @}
 */

#include <wolfssl/ssl.h>
#include <sock_tls.h>

#include <stdio.h>
#include <inttypes.h>

#include <net/sock/udp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PORT 11111
#define DEBUG 1
extern const unsigned char server_cert[];
extern const unsigned char server_key[];
extern unsigned int server_cert_len;
extern unsigned int server_key_len;

static sock_tls_t skv;
static sock_tls_t *sk = &skv;

static const char Test_dtls_string[] = "DTLS OK!";

#define APP_DTLS_BUF_SIZE 64
int dtls_server(int argc, char **argv)
{
    char buf[APP_DTLS_BUF_SIZE];
    int ret;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = SERVER_PORT;

    (void)argc;
    (void)argv;

    if (sock_dtls_create(sk, &local, NULL, 0, wolfDTLSv1_2_server_method()) != 0) {
        printf("ERROR: Unable to create DTLS sock\r\n");
        return -1;
    }

    /* Load certificate file for the DTLS server */
    if (wolfSSL_CTX_use_certificate_buffer(sk->ctx, server_cert,
                server_cert_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
    {
        printf("Failed to load certificate from memory.\r\n");
        return -1;
    }

    /* Load the private key */
    if (wolfSSL_CTX_use_PrivateKey_buffer(sk->ctx, server_key,
                server_key_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
    {
        printf("Failed to load private key from memory.\r\n");
        return -1;
    }

    /* Create the DTLS session */
    ret = sock_dtls_session_create(sk);
    if (ret < 0)
    {
        printf("Failed to create DTLS session (err: %s)\r\n", strerror(-ret));
        return -1;
    }

    printf("Listening on %d\n", SERVER_PORT);
    while(1) {

        /* Wait until a new client connects */
        ret = wolfSSL_accept(sk->ssl);
        if (ret != SSL_SUCCESS) {
            continue;
        }

        /* Wait until data is received */
        printf("Connection accepted\r\n");
        ret = wolfSSL_read(sk->ssl, buf, APP_DTLS_BUF_SIZE);
        if (ret > 0) {
            buf[ret] = (char)0;
            printf("Received '%s'\r\n", buf);
        }

        /* Send reply */
        printf("Sending 'DTLS OK'...\r\n");
        wolfSSL_write(sk->ssl, Test_dtls_string, sizeof(Test_dtls_string));

        /* Cleanup/shutdown */
        printf("Closing connection.\r\n");
        sock_dtls_session_destroy(sk);
        sock_dtls_close(sk);
        break;
    }
    return 0;
}
