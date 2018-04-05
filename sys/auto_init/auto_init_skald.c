/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 *
 * @file
 * @brief       Skald's auto-init code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "log.h"
#include "net/skald.h"

/* include fitting radio driver */
#if defined(MODULE_NRFBLE)
#include "nrfble.h"
/* add more once implemented... */
#else
#error "[auto_init_skald] error: unable to find any netdev-ble capable radio"
#endif

void auto_init_skald(void)
{
    netdev_t *dev = NULL;

    /* select radio depending on selected hardware */
#if defined(MODULE_NRFBLE)
    LOG_DEBUG("[auto_init_skald] selecting nrfble\n");
    dev = nrfble_setup();
    /* add any future netdev-ble capable radios here... */
#endif

    skald_init(dev);
}
