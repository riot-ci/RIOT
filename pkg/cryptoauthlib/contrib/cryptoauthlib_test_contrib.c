/*
 * Copyright (C) 2019 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib cryptoauthlib security crypto test
 * @{
 *
 * @file
 * @brief       Helper function for CryptoAuth unittests
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "cryptoauthlib.h"
#include "atca.h"
#include "atca_params.h"

/* Function switches the default cfg in cryptoauthlib test to Riot cfg */
void riot_switch_cfg(ATCAIfaceCfg *cfg)
{
    *cfg = atca_params[0];
}
