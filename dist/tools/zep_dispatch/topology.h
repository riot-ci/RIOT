/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool flat;
    list_node_t nodes;
    list_node_t edges;
} topology_t;

int topology_parse(const char *file, topology_t *out);

int topology_print(const char *file_out, const topology_t *t);

bool topology_add(topology_t *t, const uint8_t *mac, uint8_t mac_len,
                  struct sockaddr_in6 *addr);

void topology_send(const topology_t *t, int sock,
                   const uint8_t *mac_src, size_t mac_src_len,
                   void *buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* TOPOLOGY_H */
