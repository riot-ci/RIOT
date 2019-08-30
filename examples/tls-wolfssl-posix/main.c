/*
 * Copyright (C) 2019 Daniele Lacamera
 *
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
 * @brief       Example application using wolfSSL
 *
 * @author      Daniele Lacamera <daniele@wolfssl.com>
 * @author      Kaleb J. Himes <kaleb@wolfssl.com>
 *
 * @}
 */

#include <stdio.h>
#include <wolfssl/ssl.h>

#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "msg.h"

#include "net/sock/tcp.h"
#include "xtimer.h"


static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        printf("%s_%02u: ", iface->name, iface->num);
#ifdef MODULE_LWIP_IPV6
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i])) {
                printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
                                                       sizeof(addrstr)));
            }
        }
#endif
        puts("");
    }
    return 0;
}


#ifndef MODULE_POSIX_SOCKETS
#   error RIOT-OS lacks support for posix sockets, and this TLS app is configured to use them. Please ensure that MODULE_POSIX_SOCKETS is enabled in your configuration.
#endif


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int tls_client(int argc, char **argv);
extern int tls_server(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "tlsc", "Start a TLS client", tls_client },
    { "tlss", "Start and stop a TLS server", tls_server },
    { "ifconfig", "Shows assigned IP addresses", ifconfig },
    { NULL, NULL, NULL }
};

static void add_site_local_address(struct netif *iface)
{
    #define SITE_LOCAL_PREFIX 0xBBAAC0FE
    ip6_addr_t sl_addr;
    memcpy(&sl_addr, &iface->ip6_addr[0], sizeof(ip6_addr_t));
    sl_addr.addr[0] = SITE_LOCAL_PREFIX;
    netif_add_ip6_address(iface, &sl_addr, NULL);
}

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT wolfSSL TLS testing implementation");

    /* Initialize TCP/IP stack */
    xtimer_init();
    lwip_bootstrap();

    /* Add site-local address */
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        if (strncmp(iface->name, "lo", 2) != 0)
            add_site_local_address(iface);
    }

    /* Initialize wolfSSL */
    wolfSSL_Init();
    wolfSSL_Debugging_ON();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
