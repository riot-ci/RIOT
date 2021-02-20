/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef ZEP_PARSER_H
#define ZEP_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool zep_parse_mac(const void *buffer, size_t len, void *out, uint8_t *out_len);

void zep_set_lqi(void *buffer, uint8_t lqi);

#ifdef __cplusplus
}
#endif

#endif /* ZEP_PARSER_H */
