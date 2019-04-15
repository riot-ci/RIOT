/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"


int main(void)
{
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(
            GNRC_NETREG_DEMUX_CTX_ALL,
            gnrc_pktdump_pid
        );
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
    return 0;
}

/** @} */
