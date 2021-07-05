/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mbedtls_entropy
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include "mbedtls/entropy.h"

#include "entropy_sources_mbedtls_riot.h"
#include "entropy_mbedtls_riot.h"

/* Internal mbedtls entropy context for convenience functions */
static mbedtls_entropy_context ctx;

int entropy_mbedtls_riot_init(void)
{
    int ret = 0;

    mbedtls_entropy_init(&ctx);

    ret = riot_add_src_avail(&ctx);

    return ret;
}

int entropy_mbedtls_riot_retrieve(unsigned char *output, size_t len)
{
    assert(output != NULL);

    int ret;

    ret = mbedtls_entropy_func(&ctx, output, len);

    return ret;
}

void entropy_mbedtls_riot_uninit(void)
{
    mbedtls_entropy_free(&ctx);
}

int entropy_mbedtls_riot_get(unsigned char *output, size_t len)
{
    assert(output != NULL);

    int ret = 0;

    if ((ret = entropy_mbedtls_riot_init()) < 0) {
        entropy_mbedtls_riot_uninit();
        return ret;
    }

    ret = entropy_mbedtls_riot_retrieve(output, len);

    /* Always uninitialize at the end */
    entropy_mbedtls_riot_uninit();

    return ret;
}
