/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Overwriting definitions from <stdio.h>
 *
 * This file undefines the *putchar* and *getchar* macros.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef STDIO_H
#define STDIO_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include_next <stdio.h>

#undef putchar
#undef getchar

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* STDIO_H */
