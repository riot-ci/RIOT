/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       DHCPv6 client test application
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <stddef.h>

extern int _gnrc_netif_config(int argc, char **argv);

int main(void)
{
    _gnrc_netif_config(0, NULL);
    return 0;
}
