/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_autoadv
 *
 * @{
 *
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nimble_riot.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "net/bluetil/ad.h"

#include "nimble_autoadv.h"

static struct ble_gap_adv_params _advp;

static int32_t _adv_duration = BLE_HS_FOREVER;

static uint8_t buf[BLE_HS_ADV_MAX_SZ];
static bluetil_ad_t _ad;

static int _gap_event_cb(struct ble_gap_event *event, void *arg);

static ble_gap_event_fn *_gap_cb = &_gap_event_cb;
static void *_gap_cb_arg = NULL;

void nimble_autoadv_adv_start(void);

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void) arg;

    switch (event->type) {

        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status != 0) {
                // failed, ensure advertising is restarted
                nimble_autoadv_adv_start();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            nimble_autoadv_adv_start();
            break;
    }

    return 0;
}

void nimble_autoadv_init(void)
{
    memset(&_advp, 0, sizeof _advp);
    _advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    _advp.disc_mode = BLE_GAP_DISC_MODE_GEN;

    int rc = 0;
    (void) rc;

    rc = bluetil_ad_init_with_flags(&_ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    assert(rc == BLUETIL_AD_OK);

    if (NIMBLE_AUTOADV_DEVICE_NAME != NULL) {
        rc = bluetil_ad_add_name(&_ad, NIMBLE_AUTOADV_DEVICE_NAME);
        assert(rc == BLUETIL_AD_OK);
    }

    rc = ble_gap_adv_set_data(_ad.buf, _ad.pos);
    assert(rc == 0);

    if (!NIMBLE_AUTOADV_START_MANUALLY) {
        nimble_autoadv_adv_start();
    }
}

int nimble_autoadv_add_field(uint8_t type, const void *data, size_t data_len)
{
    int rc = bluetil_ad_add(&_ad, type, data, data_len);
    assert(rc == BLUETIL_AD_OK);

    if (!NIMBLE_AUTOADV_START_MANUALLY) {
        nimble_autoadv_adv_start();
    }

    return rc;
}

void nimble_autoadv_set_ble_gap_adv_params(struct ble_gap_adv_params *params)
{
    memcpy(&_advp, params, sizeof(struct ble_gap_adv_params));
}

void nimble_auto_adv_set_adv_duration(int32_t duration_ms)
{
    _adv_duration = duration_ms;
}

void nimble_auto_adv_set_gap_cb(ble_gap_event_fn *cb, void *cb_arg)
{
    int rc;
    (void) rc;

    _gap_cb = cb;
    _gap_cb_arg = cb_arg;

    if (!NIMBLE_AUTOADV_START_MANUALLY) {
        nimble_autoadv_adv_start();
    }
}

void nimble_autoadv_adv_start(void)
{
    int rc;
    (void) rc;

    rc = ble_gap_adv_stop();
    assert(rc == BLE_HS_EALREADY || rc == 0);

    rc = ble_gap_adv_set_data(_ad.buf, _ad.pos);
    assert(rc == 0);

    rc = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, _adv_duration, &_advp, _gap_cb, _gap_cb_arg);
    assert(rc == 0);
}
