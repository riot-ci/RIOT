/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_credman
 * @{
 *
 * @file
 * @brief       (D)TLS Credentials management module implementation
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include "net/credman.h"

#include <string.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

static credman_credential_t credentials[CREDMAN_MAX_CREDENTIALS];
static unsigned used = 0;

static int _find_credential_pos(credman_tag_t tag, credman_type_t type);
static int _find_next_free_pos(void);

int credman_add(const credman_credential_t *credential)
{
    assert(credential);
    int pos = -1;

    if ((credential->type == CREDMAN_TYPE_EMPTY) ||
        (credential->tag == CREDMAN_TAG_EMPTY)) {
        DEBUG("credman: invalid credential type/tag\n");
        return CREDMAN_INVALID;
    }
    switch (credential->type) {
    case CREDMAN_TYPE_PSK:
        if (credential->params.psk == NULL) {
            DEBUG("credman: no PSK credential found\n");
            return CREDMAN_INVALID;
        }
        break;
    case CREDMAN_TYPE_ECDSA:
        if (credential->params.ecdsa == NULL) {
            DEBUG("credman: no ECDSA credential found\n");
            return CREDMAN_ERROR;
        }
        if ((credential->params.ecdsa->private_key == NULL) ||
            (credential->params.ecdsa->public_key.x == NULL) ||
            (credential->params.ecdsa->public_key.y == NULL)) {
            DEBUG("credman: invalid ECDSA parameters\n");
            return CREDMAN_INVALID;
        }
        break;
    default:
        return CREDMAN_TYPE_UNKNOWN;
    }
    pos = _find_credential_pos(credential->tag, credential->type);
    if (pos >= 0) {
        DEBUG("credman: credential with tag %d and type %d already exist\n",
              credential->tag, credential->type);
        return CREDMAN_EXIST;
    }
    pos = _find_next_free_pos();
    if (pos < 0) {
        DEBUG("credman: no space for new credential\n");
        return CREDMAN_NO_SPACE;
    }

    credentials[pos] = *credential;
    used++;
    return CREDMAN_OK;
}

int credman_get(credman_credential_t *credential,credman_tag_t tag,
                credman_type_t type)
{
    assert(credential);

    int pos = _find_credential_pos(tag, type);
    if (pos < 0) {
        DEBUG("credman: credential with tag %d and type %d not found\n",
               tag, type);
        return CREDMAN_NOT_FOUND;
    }
    memcpy(credential, &credentials[pos], sizeof(credman_credential_t));
    return CREDMAN_OK;
}

int credman_delete(credman_tag_t tag, credman_type_t type)
{
    int pos = _find_credential_pos(tag, type);
    if (pos < 0) {
        DEBUG("credman: credential with tag %d and type %d not found\n",
               tag, type);
        return CREDMAN_NOT_FOUND;
    }
    memset(&credentials[pos], 0, sizeof(credman_credential_t));
    used--;
    return CREDMAN_OK;
}

int credman_get_used_count(void)
{
    return used;
}

static int _find_credential_pos(credman_tag_t tag, credman_type_t type)
{
    for (unsigned i = 0; i < CREDMAN_MAX_CREDENTIALS; i++) {
        credman_credential_t *c = &credentials[i];
        if ((c->tag == tag) && (c->type == type)) {
            return i;
        }
    }
    return -1;
}

static int _find_next_free_pos(void)
{
    for (int i = 0; i < CREDMAN_MAX_CREDENTIALS; i++) {
        credman_credential_t *c = &credentials[i];
        if ((c->type == CREDMAN_TYPE_EMPTY) &&
            (c->tag == CREDMAN_TAG_EMPTY)) {
            return i;
        }
    }
    return (used == 0) ? 0 : -1;
}

#ifdef TEST_SUITES
void credman_reset(void)
{
    memset(credentials, 0,
           sizeof(credman_credential_t) * CREDMAN_MAX_CREDENTIALS);
    used = 0;
}
#endif /* TEST_SUITES */
