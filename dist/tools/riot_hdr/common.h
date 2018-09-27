/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <sys/types.h>

#ifdef __cplusplus
 extern "C" {
#endif

int to_file(const char *filename, void *buf, size_t len);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* COMMON_H */
