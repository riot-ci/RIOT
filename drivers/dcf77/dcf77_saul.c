/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dcf77
 * @{
 *
 * @file
 * @brief       SAUL adaption for DCF77 devices
 *
 * The values exported to SAUL for DHT devices are buffered, meaning that new
 * values will only be read from the device, if the buffered values are older
 * then 1 second (being exactly the maximum sampling time for DHT devices).
 *
 * This buffering does further introduce a coupling between both SAUL endpoints,
 * meaning that if you read from both endpoints after each other, both values
 * are from the same sensor reading.
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "dcf77.h"


static int read_data(const void *dev, phydat_t *res)
{
    if (dcf77_read((dcf77_t *)dev, (struct tm *)res)) {
        return -ECANCELED;
    }
    res->scale = -1;
    return 1;
}

const saul_driver_t dcf77_saul_driver = {
    .read = read_data,
    .write = saul_notsup,
    .type = SAUL_CLASS_ANY
};
