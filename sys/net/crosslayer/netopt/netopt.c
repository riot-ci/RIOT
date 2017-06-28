/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_netopt
 * @file
 * @brief       This file contains functionality to map netopt option
 *              numbers to strings
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *_netopt_strmap[] = {
    [NETOPT_CHANNEL]               = "NETOPT_CHANNEL",
    [NETOPT_IS_CHANNEL_CLR]        = "NETOPT_IS_CHANNEL_CLR",
    [NETOPT_ADDRESS]               = "NETOPT_ADDRESS",
    [NETOPT_ADDRESS_LONG]          = "NETOPT_ADDRESS_LONG",
    [NETOPT_ADDR_LEN]              = "NETOPT_ADDR_LEN",
    [NETOPT_SRC_LEN]               = "NETOPT_SRC_LEN",
    [NETOPT_NID]                   = "NETOPT_NID",
    [NETOPT_IPV6_IID]              = "NETOPT_IPV6_IID",
    [NETOPT_TX_POWER]              = "NETOPT_TX_POWER",
    [NETOPT_MAX_PACKET_SIZE]       = "NETOPT_MAX_PACKET_SIZE",
    [NETOPT_PRELOADING]            = "NETOPT_PRELOADING",
    [NETOPT_PROMISCUOUSMODE]       = "NETOPT_PROMISCUOUSMODE",
    [NETOPT_AUTOACK]               = "NETOPT_AUTOACK",
    [NETOPT_ACK_REQ]               = "NETOPT_ACK_REQ",
    [NETOPT_RETRANS]               = "NETOPT_RETRANS",
    [NETOPT_PROTO]                 = "NETOPT_PROTO",
    [NETOPT_STATE]                 = "NETOPT_STATE",
    [NETOPT_RAWMODE]               = "NETOPT_RAWMODE",
    [NETOPT_RX_START_IRQ]          = "NETOPT_RX_START_IRQ",
    [NETOPT_RX_END_IRQ]            = "NETOPT_RX_END_IRQ",
    [NETOPT_TX_START_IRQ]          = "NETOPT_TX_START_IRQ",
    [NETOPT_TX_END_IRQ]            = "NETOPT_TX_END_IRQ",
    [NETOPT_AUTOCCA]               = "NETOPT_AUTOCCA",
    [NETOPT_CSMA]                  = "NETOPT_CSMA",
    [NETOPT_CSMA_RETRIES]          = "NETOPT_CSMA_RETRIES",
    [NETOPT_IS_WIRED]              = "NETOPT_IS_WIRED",
    [NETOPT_DEVICE_TYPE]           = "NETOPT_DEVICE_TYPE",
    [NETOPT_CHANNEL_PAGE]          = "NETOPT_CHANNEL_PAGE",
    [NETOPT_CCA_THRESHOLD]         = "NETOPT_CCA_THRESHOLD",
    [NETOPT_CCA_MODE]              = "NETOPT_CCA_MODE",
    [NETOPT_STATS]                 = "NETOPT_STATS",
    [NETOPT_ENCRYPTION]            = "NETOPT_ENCRYPTION",
    [NETOPT_ENCRYPTION_KEY]        = "NETOPT_ENCRYPTION_KEY",
    [NETOPT_RF_TESTMODE]           = "NETOPT_RF_TESTMODE",
    [NETOPT_L2FILTER]              = "NETOPT_L2FILTER",
    [NETOPT_L2FILTER_RM]           = "NETOPT_L2FILTER_RM",
    [NETOPT_LAST_ED_LEVEL]         = "NETOPT_LAST_ED_LEVEL",
    [NETOPT_MODEM_TYPE]            = "NETOPT_MODEM_TYPE",
    [NETOPT_BANDWIDTH]             = "NETOPT_BANDWIDTH",
    [NETOPT_SPREADING_FACTOR]      = "NETOPT_SPREADING_FACTOR",
    [NETOPT_CODING_RATE]           = "NETOPT_CODING_RATE",
    [NETOPT_SINGLE_RECEIVE]        = "NETOPT_SINGLE_RECEIVE",
    [NETOPT_RX_TIMEOUT]            = "NETOPT_RX_TIMEOUT",
    [NETOPT_TX_TIMEOUT]            = "NETOPT_TX_TIMEOUT",
    [NETOPT_PREAMBLE_LENGTH]       = "NETOPT_PREAMBLE_LENGTH",
    [NETOPT_CRC]                   = "NETOPT_CRC",
    [NETOPT_RANDOM]                = "NETOPT_RANDOM",
    [NETOPT_FREQUENCY_HOP]         = "NETOPT_FREQUENCY_HOP",
    [NETOPT_FREQUENCY_HOP_PERIOD]  = "NETOPT_FREQUENCY_HOP_PERIOD",
    [NETOPT_FIXED_HEADER]          = "NETOPT_FIXED_HEADER",
    [NETOPT_IQ_INVERT]             = "NETOPT_IQ_INVERT",
    [NETOPT_PAYLOAD_LENGTH]        = "NETOPT_PAYLOAD_LENGTH",
    [NETOPT_NUMOF]                 = "NETOPT_NUMOF",
};

const char *netopt2str(netopt_t opt)
{
    if (opt >= NETOPT_NUMOF) {
        return "unknown";
    }
    else {
        const char *tmp = _netopt_strmap[opt];
        return tmp ? tmp : "unknown";
    }
}

#ifdef __cplusplus
}
#endif
